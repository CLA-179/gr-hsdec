/* -*- c++ -*- */
/*
 * Copyright 2025 gr-hsdec author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_HSDEC_VARIABLE_PACK_ENC_IMPL_H
#define INCLUDED_HSDEC_VARIABLE_PACK_ENC_IMPL_H

#include <gnuradio/hsdec/variable_pack_enc.h>
#include <deque>
namespace gr {
namespace hsdec {

class variable_pack_enc_impl : public variable_pack_enc
{
private:
    // Nothing to declare in this block.
    std::string d_call;
    pmt::pmt_t d_in_port;
    #define HEAD_LEN (4+6+2)
    uint32_t d_head_sync = 0x1acffc1d;

    int d_pre_len = 200;
    bool d_sending = false;

    void pmt_in_callback(pmt::pmt_t msg);
    typedef struct 
    {
        uint8_t* p_data;
        uint16_t len;
    } PACK_t;

    std::deque<PACK_t*> fifo;

public:
    variable_pack_enc_impl(std::string call, int preamble_len);
    ~variable_pack_enc_impl();

    // Where all the action really happens
    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);
};

} // namespace hsdec
} // namespace gr

#endif /* INCLUDED_HSDEC_VARIABLE_PACK_ENC_IMPL_H */
