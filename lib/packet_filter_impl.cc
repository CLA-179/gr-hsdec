/* -*- c++ -*- */
/*
 * Copyright 2025 gr-hsdec author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "packet_filter_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace hsdec {

// #pragma message("set the following appropriately and remove this warning")
using input_type = unsigned char;
// #pragma message("set the following appropriately and remove this warning")
using output_type = unsigned char;
packet_filter::sptr packet_filter::make(int chunk_len, int sps)
{
    return gnuradio::make_block_sptr<packet_filter_impl>(chunk_len, sps);
}


/*
 * The private constructor
 */
packet_filter_impl::packet_filter_impl(int chunk_len, int sps)
    : gr::block("packet_filter",
                gr::io_signature::make(
                    1 /* min inputs */, 1 /* max inputs */, sizeof(input_type)),
                gr::io_signature::make(
                    1 /* min outputs */, 1 /*max outputs */, sizeof(output_type))),
      d_chunk_len(chunk_len),
      d_sps(sps)
{
    set_tag_propagation_policy(TPP_DONT); // 自己控制 tag 传递
}

void packet_filter_impl::set_size(int size)
{
    d_chunk_len = size;
    set_output_multiple(d_chunk_len);
    set_relative_rate(static_cast<uint64_t>(d_chunk_len * 2),
                      static_cast<uint64_t>(d_chunk_len/8));
}

/*
 * Our virtual destructor.
 */
packet_filter_impl::~packet_filter_impl() {}

void packet_filter_impl::forecast(int noutput_items, gr_vector_int& ninput_items_required)
{
    ninput_items_required[0] =
        std::max(d_chunk_len / d_sps * 2, noutput_items*8); // 保障输入充足
}

int packet_filter_impl::general_work(int noutput_items,
                                     gr_vector_int& ninput_items,
                                     gr_vector_const_void_star& input_items,
                                     gr_vector_void_star& output_items)
{
    const uint8_t* in = static_cast<const uint8_t*>(input_items[0]);
    uint8_t* out = static_cast<uint8_t*>(output_items[0]);
    const int input_len = ninput_items[0];

    std::vector<gr::tag_t> tags;
    get_tags_in_range(tags, 0, nitems_read(0), nitems_read(0) + input_len);

    // 找出所有 packet_len 标签
    std::vector<uint64_t> packet_starts;
    for (auto& tag : tags) {
        if (pmt::symbol_to_string(tag.key) == "packet_len") {
            packet_starts.push_back(tag.offset);
        }
    }

    // 如果没有完整的一对 packet_len，先不处理
    if (packet_starts.size() < 2)
        return 0;

    // 提取第一个 packet_len 区间
    uint64_t start = packet_starts[0];
    uint64_t end = packet_starts[1];
    uint64_t valid_start = end - (d_chunk_len / d_sps);

    // 如果流还没到 end，就等待
    if (nitems_read(0) + input_len < end)
        return 0;

    // 找出 sync_head 标签（包含帧起始与长度）
    std::vector<gr::tag_t> sync_tags;
    get_tags_in_range(sync_tags, 0, start, end);

    struct FrameInfo {
        uint64_t offset;
        int length;
    };

    std::vector<FrameInfo> valid_frames;

    for (auto& tag : sync_tags) {
        if (pmt::symbol_to_string(tag.key) == "sync_head") {
            uint64_t pos = tag.offset;
            int len = pmt::to_long(tag.value);

            uint64_t frame_start = pos;
            uint64_t frame_end = pos + len;

            if (frame_end > end)
                continue; // 帧超出区域末尾，丢弃

            // 只要帧有一部分进入有效区域（特别是 frame_end > valid_start）
            if (frame_end > valid_start) {
                valid_frames.push_back({ frame_start, len });
            }
        }
    }

    // 将这些帧复制到输出
    int out_idx = 0;
    // for (auto &f : valid_frames) {
    //     for (int i = 0; i < f.length; ++i) {
    //         if (out_idx >= noutput_items) break;
    //         out[out_idx++] = in[f.offset - nitems_read(0) + i];
    //     }
    // }

    for (auto& f : valid_frames) {
        int rel_offset = f.offset - nitems_read(0); // 相对当前输入的偏移
        if (rel_offset < 0 || rel_offset + f.length > input_len)
            continue; // 越界保护

        // 拷贝数据帧
        for (int i = 0; i < f.length && out_idx < noutput_items; ++i) {
            out[out_idx] = (out[out_idx] << 1) |  (in[rel_offset + i] & 0x1);
            if (i%8 == 7)
            {
                out_idx++;
            }
            
        }

        // 复制 sync_head tag 到输出流对应位置
        add_item_tag(0,
                     nitems_written(0) + out_idx - (f.length / 8),
                     pmt::string_to_symbol("sync_head"),
                     pmt::from_long(f.length / 8));
    }


    // 打 tag: packet_len 到第一个保留帧
    if (!valid_frames.empty()) {
        add_item_tag(0,
                     nitems_written(0),
                     pmt::string_to_symbol("packet_len"),
                     pmt::from_long(out_idx));
    }

    // 消费掉第一个 packet_len 区段数据
    consume(0, end - nitems_read(0));
    return out_idx;
}

} /* namespace hsdec */
} /* namespace gr */
