/* -*- c++ -*- */
/*
 * Copyright 2025 gr-hsdec author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "frame_sync_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace hsdec {

// #pragma message("set the following appropriately and remove this warning")
using input_type = unsigned char;
// #pragma message("set the following appropriately and remove this warning")
using output_type = unsigned char;
frame_sync::sptr frame_sync::make(uint32_t code, int len, int block_size)
{
    return gnuradio::make_block_sptr<frame_sync_impl>(code, len, block_size);
}


/*
 * The private constructor
 */
frame_sync_impl::frame_sync_impl(uint32_t code, int len, int block_size)
    : gr::block("frame_sync",
                gr::io_signature::make(
                    1 /* min inputs */, 1 /* max inputs */, sizeof(input_type)),
                gr::io_signature::make(
                    1 /* min outputs */, 1 /*max outputs */, sizeof(output_type)))
{
    g_code = code;
    g_block_size = block_size;
    g_len = len;

    set_output_multiple(block_size);

    printf("sync code : %#08X\n", g_code);
}

/*
 * Our virtual destructor.
 */
frame_sync_impl::~frame_sync_impl() {}

void frame_sync_impl::forecast(int noutput_items, gr_vector_int& ninput_items_required)
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

int frame_sync_impl::general_work(int noutput_items,
                                  gr_vector_int& ninput_items,
                                  gr_vector_const_void_star& input_items,
                                  gr_vector_void_star& output_items)
{
    const unsigned char* in = static_cast<const unsigned char*>(input_items[0]);
    unsigned char* out = static_cast<unsigned char*>(output_items[0]);

    uint64_t abs_offset = nitems_read(0);

    for (int i = 0; i < noutput_items; ++i) {
        out[i] = in[i];

        if (d_skip_count > 0) {
            d_skip_count--;
            continue;
        }

        d_data_reg = ((d_data_reg << 1) | (in[i] & 0x1));

        if (d_data_reg_bits < 32) {
            d_data_reg_bits++;
            continue;
        }
        

        // Full 32 bits in shift register
        uint64_t wrong_bits = (d_data_reg ^ g_code) & d_mask;
        uint64_t nwrong =  count_one32(wrong_bits);
       

        if (nwrong <= d_threshold) {
            // printf("sync:%ld\n", abs_offset + i - 32 + 1);
            // Match found
            add_item_tag(
                0,                      // 输出端口
                abs_offset + i - 32 + 1,    // tag 位置
                pmt::intern("sync_head"),
                pmt::from_long(g_len*8+32)
            );
            d_skip_count = g_len * 8;
        }
        else if (nwrong >= 32 - d_threshold) {
            // printf("sync:%ld\n", abs_offset + i - 32 + 1);
            // Match found
            add_item_tag(
                0,                      // 输出端口
                abs_offset + i - 32 + 1,    // tag 位置
                pmt::intern("sync_head_flip"),
                pmt::from_long(g_len*8+32)
            );
            
            d_skip_count = g_len * 8;
        }
    }

    consume_each(noutput_items);
    return noutput_items;
}

} /* namespace hsdec */
} /* namespace gr */
