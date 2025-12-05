/* -*- c++ -*- */
/*
 * Copyright 2025 gr-hsdec author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_HSDEC_VARIABLE_PACK_DEC_IMPL_H
#define INCLUDED_HSDEC_VARIABLE_PACK_DEC_IMPL_H

#include <gnuradio/hsdec/variable_pack_dec.h>

namespace gr {
namespace hsdec {

class variable_pack_dec_impl : public variable_pack_dec
{
private:
    // Nothing to declare in this block.
    std::string d_call_pass;
    pmt::pmt_t d_out_port;
#define HEAD_LEN 4 + 6 + 2
    uint32_t d_head_sync = 0x1acffc1d;

    int d_order = 4;

    uint32_t d_code;
    uint16_t d_len = 0;
    int d_block_size;

    int d_skip_count = 0;
    uint32_t d_data_reg = 0;
    uint32_t d_mask = 0xFFFFFFFF;
    uint32_t d_threshold = 1;
    int d_data_reg_bits = 0;

    bool d_flip_2;
    bool d_sync_flag = false;
    bool d_len_flag = false;
    uint16_t d_len_index = 0;
    bool d_call = false;
    bool d_data = false;
    int d_call_index = 0;

    unsigned char call_recv[7];

    std::vector<unsigned char> decoded_data;
    int d_data_index;
    uint8_t d_data_bit_mask;

    int decoder(std::vector<uint8_t> &data);

public:
    variable_pack_dec_impl(std::string call, int order);
    ~variable_pack_dec_impl();

    // Where all the action really happens
    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);
};

} // namespace hsdec
} // namespace gr

#endif /* INCLUDED_HSDEC_VARIABLE_PACK_DEC_IMPL_H */
