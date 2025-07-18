/* -*- c++ -*- */
/*
 * Copyright 2025 gr-hsdec author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_HSDEC_PACKET_FILTER_IMPL_H
#define INCLUDED_HSDEC_PACKET_FILTER_IMPL_H

#include <gnuradio/hsdec/packet_filter.h>

namespace gr {
namespace hsdec {

class packet_filter_impl : public packet_filter
{
private:
    // Nothing to declare in this block.
    int d_chunk_len;
    int d_sps;

    std::vector<uint8_t> d_buffer;
    uint64_t d_start_offset = 0;

public:
    packet_filter_impl(int chunk_len, int sps);
    ~packet_filter_impl();

    void set_size(int size) override;

    // Where all the action really happens
    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);
};

} // namespace hsdec
} // namespace gr

#endif /* INCLUDED_HSDEC_PACKET_FILTER_IMPL_H */
