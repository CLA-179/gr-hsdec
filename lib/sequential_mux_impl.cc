/* -*- c++ -*- */
/*
 * Copyright 2025 gr-hsdec author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "sequential_mux_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace hsdec {

// #pragma message("set the following appropriately and remove this warning")
using input_type = unsigned char;
// #pragma message("set the following appropriately and remove this warning")
using output_type = unsigned char;
sequential_mux::sptr sequential_mux::make(int n)
{
    return gnuradio::make_block_sptr<sequential_mux_impl>(n);
}


/*
 * The private constructor
 */
sequential_mux_impl::sequential_mux_impl(int n)
    : gr::block("sequential_mux",
                gr::io_signature::make(
                    4 /* min inputs */, 4 /* max inputs */, sizeof(input_type)),
                gr::io_signature::make(
                    1 /* min outputs */, 1 /*max outputs */, sizeof(output_type)))
{
    block_size = n;
    set_output_multiple(n*4);
    set_relative_rate(static_cast<uint64_t>(block_size*4), static_cast<uint64_t>(block_size));
}

/*
 * Our virtual destructor.
 */
sequential_mux_impl::~sequential_mux_impl() {}


void sequential_mux_impl::set_n(int n)
{
    block_size = n;
    set_output_multiple(n*4);
    set_relative_rate(static_cast<uint64_t>(block_size*4), static_cast<uint64_t>(block_size));
}

void sequential_mux_impl::forecast(int noutput_items,
                                   gr_vector_int& ninput_items_required)
{

    // 每个输出 block 需要每个输入端提供 block_size 个样本
    int blocks_needed = noutput_items / (4 * block_size);

    if (blocks_needed < 1)
        blocks_needed = 1;

    for (int i = 0; i < 4; i++) {
        ninput_items_required[i] = blocks_needed * block_size;
    }
}

int sequential_mux_impl::general_work(int noutput_items,
                                      gr_vector_int& ninput_items,
                                      gr_vector_const_void_star& input_items,
                                      gr_vector_void_star& output_items)
{
    const uint8_t* in0 = (const uint8_t*)input_items[0];
    const uint8_t* in1 = (const uint8_t*)input_items[1];
    const uint8_t* in2 = (const uint8_t*)input_items[2];
    const uint8_t* in3 = (const uint8_t*)input_items[3];
    uint8_t* out = (uint8_t*)output_items[0];

    // const int block_size = 70000;
    int num_blocks = std::min({
        ninput_items[0] / block_size,
        ninput_items[1] / block_size,
        ninput_items[2] / block_size,
        ninput_items[3] / block_size,
        noutput_items / (4 * block_size)
    });

    // printf("%d\n",ninput_items[0]);

    if (num_blocks == 0) {
        // 数据不足，等待
        // consume per port
        for (int i = 0; i < 4; i++) {
            consume(i, 0);
        }
        return 0;
    }

    for (int i = 0; i < num_blocks; i++) {
        int in_offset = i * block_size;
        int out_offset = i * block_size * 4;

        memcpy(out + out_offset,                    in0 + in_offset, block_size);
        memcpy(out + out_offset + 1 * block_size,   in1 + in_offset, block_size);
        memcpy(out + out_offset + 2 * block_size,   in2 + in_offset, block_size);
        memcpy(out + out_offset + 3 * block_size,   in3 + in_offset, block_size);
    }

    // consume per port
    for (int i = 0; i < 4; i++) {
        consume(i, num_blocks * block_size);
    }

    return num_blocks * 4 * block_size; // 输出样本数量
}

} /* namespace hsdec */
} /* namespace gr */
