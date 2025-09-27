/* -*- c++ -*- */
/*
 * Copyright 2025 gr-hsdec author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_HSDEC_SQUELCH_NEW_IMPL_H
#define INCLUDED_HSDEC_SQUELCH_NEW_IMPL_H

#include <gnuradio/hsdec/squelch_new.h>

namespace gr {
namespace hsdec {

class squelch_new_impl : public squelch_new
{
private:
    // Nothing to declare in this block.
    float d_thre;
    int zero_count = 0;

public:
    squelch_new_impl(float);
    ~squelch_new_impl();

    // Where all the action really happens
    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);
};

} // namespace hsdec
} // namespace gr

#endif /* INCLUDED_HSDEC_SQUELCH_NEW_IMPL_H */
