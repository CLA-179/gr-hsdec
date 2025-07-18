/* -*- c++ -*- */
/*
 * Copyright 2025 gr-hsdec author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_HSDEC_SEQUENTIAL_MUX_IMPL_H
#define INCLUDED_HSDEC_SEQUENTIAL_MUX_IMPL_H

#include <gnuradio/hsdec/sequential_mux.h>

namespace gr {
namespace hsdec {

class sequential_mux_impl : public sequential_mux
{
private:
    // Nothing to declare in this block.
    int block_size;

public:
    sequential_mux_impl(int n);
    ~sequential_mux_impl();

    // Where all the action really happens
    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);

    void set_n(int n) override;
};

} // namespace hsdec
} // namespace gr

#endif /* INCLUDED_HSDEC_SEQUENTIAL_MUX_IMPL_H */
