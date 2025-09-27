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

    // 防护：确保 ninput_items 至少有一个元素
    if (ninput_items.size() == 0) {
        return 0;
    }

    std::vector<gr::tag_t> tags;
    get_tags_in_range(tags, 0, nitems_read(0), nitems_read(0) + input_len);

    std::vector<uint64_t> packet_starts;
    for (auto& tag : tags) {
        if (pmt::symbol_to_string(tag.key) == "packet_len") {
            packet_starts.push_back(tag.offset);
        }
    }

    // 如果没有 packet_len 标签
    if (packet_starts.empty())
    {
        // 没有任何 packet_len，安全消费掉全部已读输入（或按需改成不消费）
        consume_each(input_len);
        return 0;
    }

    // 如果只有一个 packet_len，尝试消费其之前的数据（但不要越界）
    if (packet_starts.size() < 2)
    {
        uint64_t first = packet_starts[0];
        if (first > nitems_read(0)) {
            uint64_t to_consume = first - nitems_read(0);
            // 不超过当前可用输入
            to_consume = std::min<uint64_t>(to_consume, static_cast<uint64_t>(input_len));
            consume(0, static_cast<int>(to_consume));
        }
        return 0;
    }

    uint64_t start = packet_starts[0];
    uint64_t end = packet_starts[1];
    uint64_t valid_start = (end > static_cast<uint64_t>(d_chunk_len / d_sps)) ?
                            end - static_cast<uint64_t>(d_chunk_len / d_sps) : 0;

    // 查找 sync_head 范围（start..end）
    std::vector<gr::tag_t> sync_tags;
    get_tags_in_range(sync_tags, 0, start, end);

    std::vector<FrameInfo> valid_frames;
    for (auto& tag : sync_tags) {
        std::string k = pmt::symbol_to_string(tag.key);
        if (k == "sync_head" || k == "sync_head_flip") {
            uint64_t pos = tag.offset;
            // 防护：确保 tag.value 可转为 long
            if (!pmt::is_number(tag.value)) continue;
            int len = pmt::to_long(tag.value);
            if (len <= 0) continue;

            uint64_t frame_start = pos;
            uint64_t frame_end = pos + static_cast<uint64_t>(len);

            if (frame_end > end) continue; // 超出 end 丢弃

            if (frame_end > valid_start) {
                bool flip = (k == "sync_head_flip");
                valid_frames.push_back({ frame_start, len, flip });
            }
        }
    }

    int out_idx = 0;
    // 将每帧按位构造字节并写入 out
    for (auto& f : valid_frames) {
        int64_t rel_offset = static_cast<int64_t>(f.offset) - static_cast<int64_t>(nitems_read(0));
        if (rel_offset < 0 || rel_offset + f.length > input_len) {
            // 如果帧数据超出当前输入缓冲，跳过（或你也许想部分保留——视设计）
            continue;
        }

        // 用一个临时字节累积 8 个 bit，然后写到 out[out_idx]
        uint8_t accum = 0;
        int bit_count = 0;
        for (int i = 0; i < f.length && out_idx < noutput_items; ++i) {
            uint8_t bit = in[rel_offset + i] & 0x1;
            accum = (accum << 1) | bit;
            ++bit_count;

            if (bit_count == 8) {
                if (f.flip_flag) accum = ~accum;
                out[out_idx++] = accum;
                // reset
                accum = 0;
                bit_count = 0;
            }
        }
        // 如果最后不整 8 bit，决定是否写入（原逻辑是按字节写，故这里我们丢弃不满一字节的位）
        // 如果你想保留部分字节，需要定义填充规则。

        // 计算要打 tag 的位置，防止负值或越界
        int frames_bytes = f.length / 8;
        if (frames_bytes > 0) {
            int tag_pos = (nitems_written(0) + out_idx) - frames_bytes;
            if (tag_pos >= static_cast<int>(nitems_written(0))) {
                add_item_tag(0,
                             static_cast<uint64_t>(tag_pos),
                             pmt::string_to_symbol("sync_head"),
                             pmt::from_long(frames_bytes));
            }
        }
    }

    if (out_idx > 0) {
        add_item_tag(0,
                     nitems_written(0),
                     pmt::string_to_symbol("packet_len"),
                     pmt::from_long(out_idx));
    }

    // 安全消费：end - nitems_read(0) 应 >= 0
    if (end > nitems_read(0)) {
        uint64_t to_consume = end - nitems_read(0);
        // 防止超过 input_len
        to_consume = std::min<uint64_t>(to_consume, static_cast<uint64_t>(input_len));
        consume(0, static_cast<int>(to_consume));
    }

    return out_idx;
}


} /* namespace hsdec */
} /* namespace gr */
