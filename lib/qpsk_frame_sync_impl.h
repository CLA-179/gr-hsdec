/* -*- c++ -*- */
/*
 * Copyright 2025 gr-hsdec author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_HSDEC_QPSK_FRAME_SYNC_IMPL_H
#define INCLUDED_HSDEC_QPSK_FRAME_SYNC_IMPL_H

#include <gnuradio/hsdec/qpsk_frame_sync.h>

namespace gr {
namespace hsdec {

class qpsk_frame_sync_impl : public qpsk_frame_sync
{
private:
    // Nothing to declare in this block.
    uint32_t g_code;
    int g_len;
    int g_block_size;

    int d_skip_count = 0;
    uint32_t d_data_reg = 0;
    uint32_t d_mask = 0xFFFFFFFF;
    uint32_t d_threshold = 1;
    int d_data_reg_bits = 0;
    uint32_t d_sync_code[4];


public:
    qpsk_frame_sync_impl(uint32_t code, int len, int block_size);
    ~qpsk_frame_sync_impl();

    // Where all the action really happens
    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);
};

} // namespace hsdec
} // namespace gr

#endif /* INCLUDED_HSDEC_QPSK_FRAME_SYNC_IMPL_H */
