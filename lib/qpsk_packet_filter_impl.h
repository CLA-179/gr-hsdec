/* -*- c++ -*- */
/*
 * Copyright 2025 gr-hsdec author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_HSDEC_QPSK_PACKET_FILTER_IMPL_H
#define INCLUDED_HSDEC_QPSK_PACKET_FILTER_IMPL_H

#include <gnuradio/hsdec/qpsk_packet_filter.h>

namespace gr {
namespace hsdec {

class qpsk_packet_filter_impl : public qpsk_packet_filter
{
private:
    // Nothing to declare in this block.
    int d_chunk_len;
    int d_sps;

    std::vector<uint8_t> d_buffer;
    uint64_t d_start_offset = 0;

    struct FrameInfo {
        uint64_t offset;
        int length;
        uint8_t flip_phi;
    };

public:
    qpsk_packet_filter_impl(int chunk_len, int sps);
    ~qpsk_packet_filter_impl();

    // Where all the action really happens
    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);
};

} // namespace hsdec
} // namespace gr

#endif /* INCLUDED_HSDEC_QPSK_PACKET_FILTER_IMPL_H */
