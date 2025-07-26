/* -*- c++ -*- */
/*
 * Copyright 2025 gr-hsdec author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "qpsk_frame_sync_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace hsdec {

// #pragma message("set the following appropriately and remove this warning")
using input_type = unsigned char;
// #pragma message("set the following appropriately and remove this warning")
using output_type = unsigned char;
qpsk_frame_sync::sptr qpsk_frame_sync::make(uint32_t code, int len, int block_size)
{
    return gnuradio::make_block_sptr<qpsk_frame_sync_impl>(code, len, block_size);
}

static const uint8_t rot_map[4][4] = 
{
    {0, 1, 2, 3},
    {1, 3, 0, 2},
    {3, 2, 1, 0},
    {2, 0, 3, 1}
};


// 输入：32位QPSK同步字（每2位是一个符号，I/Q顺序）
// 输出：rotated[0]=原始，rotated[1]=+90°，rotated[2]=+180°，rotated[3]=+270°
void rotate_qpsk_syncword_all(uint32_t sync_word, uint32_t rotated[4])
{
    for (int i = 0; i < 4; ++i)
        rotated[i] = 0;

    for (int i = 0; i < 16; ++i) {
        // 每个符号占两位，从高位开始（MSB）
        int bit_pos = 30 - i * 2;
        uint8_t sym = (sync_word >> bit_pos) & 0x3; // 提取当前2bit符号

        for (int rot = 0; rot < 4; ++rot) {
            uint8_t rsym = rot_map[rot][sym] & 0x3; // QPSK旋转 = 加mod4
            rotated[rot] |= (uint32_t)rsym << bit_pos;
        }
    }
}


/*
 * The private constructor
 */
qpsk_frame_sync_impl::qpsk_frame_sync_impl(uint32_t code, int len, int block_size)
    : gr::block("qpsk_frame_sync",
                gr::io_signature::make(
                    1 /* min inputs */, 1 /* max inputs */, sizeof(input_type)),
                gr::io_signature::make(
                    1 /* min outputs */, 1 /*max outputs */, sizeof(output_type)))
{
    g_code = code;
    g_block_size = block_size;
    g_len = len;

    set_output_multiple(block_size * 2);

    rotate_qpsk_syncword_all(g_code, d_sync_code);
    printf("sync code : %#08X\n", d_sync_code[0]);
    d_skip_count = 0;
}

/*
 * Our virtual destructor.
 */
qpsk_frame_sync_impl::~qpsk_frame_sync_impl() {}

void qpsk_frame_sync_impl::forecast(int noutput_items,
                                    gr_vector_int& ninput_items_required)
{

    ninput_items_required[0] = noutput_items;
}

static int count_one32(unsigned int in)
{
    static int out[16] = { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4 };

    int o = out[in & 0x0f];
    o += out[(in >> 4) & 0x0f];
    o += out[(in >> 8) & 0x0f];
    o += out[(in >> 12) & 0x0f];
    o += out[(in >> 16) & 0x0f];
    o += out[(in >> 20) & 0x0f];
    o += out[(in >> 24) & 0x0f];
    o += out[(in >> 28) & 0x0f];

    // printf ("%d\n", o);

    return o;
}

const std::string tag_name[4] = {
    "sync_head_0", "sync_head_90", "sync_head_180", "sync_head_270"
};

int qpsk_frame_sync_impl::general_work(int noutput_items,
                                       gr_vector_int& ninput_items,
                                       gr_vector_const_void_star& input_items,
                                       gr_vector_void_star& output_items)
{
    const unsigned char* in = static_cast<const unsigned char*>(input_items[0]);
    unsigned char* out = static_cast<unsigned char*>(output_items[0]);

    uint64_t abs_offset = nitems_read(0);

    for (int i = 0; i < noutput_items; i++) {
        out[i] = in[i];

        if (d_skip_count > 0) {
            d_skip_count--;
            continue;
        }

        d_data_reg = ((d_data_reg << 2) | (in[i] & 0x3U));

        // printf("%ld %#08X\n",abs_offset + i,d_data_reg);


        if (++d_data_reg_bits < 16) {
            // d_data_reg_bits++;
            continue;
        }


        // Full 32 bits in shift register
        uint32_t wrong_bits[4], nwrong[4];

        for (uint8_t j = 0; j < 4; j++) {
            wrong_bits[j] = (d_data_reg ^ d_sync_code[j]) & d_mask;
            nwrong[j] = count_one32(wrong_bits[j]);

            if (nwrong[j] <= d_threshold) {
                // printf("sync code : %#08X\n", d_data_reg);

                // std::cout << tag_name[j] << '\t' << abs_offset + i - 15 << std::endl;
                // Match found
                add_item_tag(0,                       // 输出端口
                             abs_offset + i - 15, // tag 位置
                             pmt::intern(tag_name[j]),
                             pmt::from_long(g_len * 4 + 16));
                d_skip_count = g_len * 4;
                break;
            }
        }
    }

    consume_each(noutput_items);
    return noutput_items;
}

} /* namespace hsdec */
} /* namespace gr */
