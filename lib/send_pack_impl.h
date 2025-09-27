/* -*- c++ -*- */
/*
 * Copyright 2025 gr-hsdec author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_HSDEC_SEND_PACK_IMPL_H
#define INCLUDED_HSDEC_SEND_PACK_IMPL_H

#include <gnuradio/hsdec/send_pack.h>

namespace gr {
namespace hsdec {

class send_pack_impl : public send_pack
{
private:
    // Nothing to declare in this block.
    pmt::pmt_t d_in_port;

    void pmt_in_callback(pmt::pmt_t msg);

public:
    send_pack_impl();
    ~send_pack_impl();

    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace hsdec
} // namespace gr

#endif /* INCLUDED_HSDEC_SEND_PACK_IMPL_H */
