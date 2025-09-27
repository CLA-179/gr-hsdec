/* -*- c++ -*- */
/*
 * Copyright 2025 gr-hsdec author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_HSDEC_LDPC_DECODER_IMPL_H
#define INCLUDED_HSDEC_LDPC_DECODER_IMPL_H

#include <gnuradio/hsdec/ldpc_decoder.h>

#include "fec/ldpc/checknode.h"
#include "fec/ldpc/variablenode.h"
#include "fec/ldpc/ldpc.h"
#include <cassert>
// #include <gnuradio/blocks/pack_k_bits.h>

namespace gr {
namespace hsdec {

class ldpc_decoder_impl : public ldpc_decoder
{
private:
    int d_iterations;
    float d_sigma;
    int d_update_sigma;
    int d_code_type;
    bool d_pack;
    // blocks::kernel::pack_k_bits *d_pack_8;
    int punct;
    string d_file;
    pmt::pmt_t d_in_port;
    pmt::pmt_t d_out_port;
    pmt::pmt_t d_err_port;
    int** H; //Parity matrix
    //double *inputData;
    ldpc *ccsdsLDPC;
    std::vector<unsigned char> decodeFullC2(std::vector<float> softBits);
    std::vector<unsigned char> decodeExpC2(std::vector<float> softBits);
    std::vector<unsigned char> decodeAR4JA(std::vector<float> softBits);
    std::vector<unsigned char> (ldpc_decoder_impl::*ldpcDecode)(std::vector<float> input);
    std::vector<unsigned char> slice(std::vector<float> logapp);
    std::vector<unsigned char> slice(std::vector<float> logapp, std::vector<float> raw_bit, float& err );

    int calc_times;
    float err_rate;

public:
    ldpc_decoder_impl(std::string alist_file, int code_type, int iterations, float sigma, int update_sigma,int pack);
    ~ldpc_decoder_impl();
    void decode(pmt::pmt_t msg);

    // Where all the action really happens
    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);
};

} // namespace hsdec
} // namespace gr

#endif /* INCLUDED_HSDEC_LDPC_DECODER_IMPL_H */
