/* -*- c++ -*- */
/*
 * Copyright 2025 gr-hsdec author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_HSDEC_RS_4_IMPL_H
#define INCLUDED_HSDEC_RS_4_IMPL_H

#include <gnuradio/hsdec/rs_4.h>

namespace gr {
namespace hsdec {

class rs_4_impl : public rs_4
{
private:
    // Nothing to declare in this block.

    pmt::pmt_t d_in_port;
    pmt::pmt_t d_out_port;

    void pmt_in_callback(pmt::pmt_t msg);

public:
    rs_4_impl();
    ~rs_4_impl();

    // Where all the action really happens
    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);
};

} // namespace hsdec
} // namespace gr

#endif /* INCLUDED_HSDEC_RS_4_IMPL_H */
