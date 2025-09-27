/* -*- c++ -*- */
/*
 * Copyright 2025 gr-hsdec author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "ldpc_decoder_impl.h"
#include <gnuradio/io_signature.h>

#include <algorithm>
#include <fstream>
#include <string>
#include <vector>

#define N 8176
#define K 7154
#define P 1022
using namespace std;

namespace gr {
namespace hsdec {

ldpc_decoder::sptr ldpc_decoder::make(std::string alist_file,
                                      int code_type,
                                      int iterations,
                                      float sigma,
                                      int update_sigma,
                                      int pack)
{
    return gnuradio::make_block_sptr<ldpc_decoder_impl>(
        alist_file, code_type, iterations, sigma, update_sigma, pack);
}


/*
 * The private constructor
 */
ldpc_decoder_impl::ldpc_decoder_impl(std::string alist_file,
                                     int code_type,
                                     int iterations,
                                     float sigma,
                                     int update_sigma,
                                     int pack)
    : gr::block("ldpc_decoder",
                gr::io_signature::make(0, 0, 0),
                gr::io_signature::make(0, 0, 0)),
      d_iterations(iterations),
      d_sigma(sigma),
      d_update_sigma(update_sigma),
      d_file(alist_file),
      d_code_type(code_type),
      d_pack(pack)
{
    // Initializing input and output ports
    d_in_port = pmt::mp("in");
    message_port_register_in(d_in_port);

    d_out_port = pmt::mp("out");
    message_port_register_out(d_out_port);

    d_err_port = pmt::mp("err");
    message_port_register_out(d_err_port);

    // Setting callback
    set_msg_handler(d_in_port, [this](const pmt::pmt_t& msg) { decode(msg); });

    // Packing bits into bytes
    // d_pack_8 = new blocks::kernel::pack_k_bits(8);


    switch (code_type) {
    case 0:
        ldpcDecode = &ldpc_decoder_impl::decodeFullC2;
        // d_file =
        // "/home/mbkitine/Dropbox/Lulea/GRC/DeepSpace/gr-ccsds/lib/fec/ldpc/alist/C2_Alist.a";
        break;
    case 1:
        ldpcDecode = &ldpc_decoder_impl::decodeExpC2;
        // d_file =
        // "/home/mbkitine/Dropbox/Lulea/GRC/DeepSpace/gr-ccsds/lib/fec/ldpc/alist/C2_Alist.a";
        break;
    case 2:
        ldpcDecode = &ldpc_decoder_impl::decodeAR4JA;
        punct = 512;
        // d_file =
        // "/home/mbkitine/Dropbox/Lulea/GRC/DeepSpace/gr-ccsds/lib/fec/ldpc/alist/AR4JA_r12_k1024n.a";
        break;
    }

    // LDPC codec object
    ccsdsLDPC = new ldpc(d_file);

    // Printing properties of the LDPC codec
    std::cout << "LDPC Decoder Initialized .." << std::endl;
    std::cout << "Code rate  : " << ccsdsLDPC->getCodeRate() << std::endl;
    std::cout << "Check nodes: " << ccsdsLDPC->getnumCheckNodes() << std::endl;
    std::cout << "Variable nodes : " << ccsdsLDPC->getnumVarNodes() << std::endl;

    set_output_multiple(N);
}

std::vector<unsigned char> ldpc_decoder_impl::decodeFullC2(std::vector<float> softBits)
{
    std::vector<float> logapp = ccsdsLDPC->decode(softBits, d_iterations, d_sigma);
    std::vector<unsigned char> decodedBits = slice(logapp);

    return decodedBits;
}

std::vector<unsigned char> ldpc_decoder_impl::decodeExpC2(std::vector<float> softBits)
{
    // Removing the last two LLRs
    softBits.pop_back();
    softBits.pop_back();

    // Insert LLRs for the first 18 elements
    std::vector<float> codeword(18, -1000.00);
    codeword.insert(codeword.end(), softBits.begin(), softBits.end());

    // LDPC Decoding
    std::vector<float> logapp = ccsdsLDPC->decode(codeword, d_iterations, d_sigma);

    // Slicing log-APP ratios into hard bits
    std::vector<unsigned char> decodedBits;
    if (++calc_times > 32)
    {
        decodedBits = slice(logapp, softBits, err_rate);
        calc_times = 0;

        std::vector<float> err_vec(1, err_rate);
        pmt::pmt_t pdu(pmt::cons(pmt::dict_add(pmt::PMT_NIL, pmt::intern("err"), pmt::from_double(err_rate)),
                                 pmt::init_f32vector(1, err_vec)));
        message_port_pub(d_err_port, pdu);
    }
    else
    {
        decodedBits = slice(logapp);
        
    }
    
    // Remove the prefixed zero bits
    decodedBits.erase(decodedBits.begin(), decodedBits.begin() + 18);

    return decodedBits;
}

std::vector<unsigned char> ldpc_decoder_impl::decodeAR4JA(std::vector<float> softBits)
{
    for (int i = 0; i < punct; i++)
        softBits.push_back(0.0);
    std::vector<float> logapp = ccsdsLDPC->decode(softBits, d_iterations, d_sigma);
    std::vector<unsigned char> decodedBits = slice(logapp);
    return decodedBits;
}

std::vector<unsigned char> ldpc_decoder_impl::slice(std::vector<float> logapp)
{
    std::vector<unsigned char> bits;
    unsigned char bit;
    for (int i = 0; i < logapp.size(); i++)
    {
        bits.push_back((logapp[i] > 0) ? 0x01 : 0x00);
    }
    return bits;
}

std::vector<unsigned char> ldpc_decoder_impl::slice(std::vector<float> logapp, std::vector<float> raw_bit, float& err )
{
    std::vector<unsigned char> bits;
    unsigned char bit;
    int err_bits = 0;
    for (int i = 0; i < logapp.size(); i++)
    {
        bits.push_back((logapp[i] > 0) ? 0x01 : 0x00);
        if (logapp[i] * raw_bit[i] < 0)
        {
            err_bits ++;
        }
    }

    err = (float)err_bits / logapp.size(); 
    return bits;
}

void ldpc_decoder_impl::decode(pmt::pmt_t msg)
{
    pmt::pmt_t meta(pmt::car(msg));
    pmt::pmt_t bits(pmt::cdr(msg));

    std::vector<uint8_t> bytes = pmt::u8vector_elements(bits);
    std::vector<float> softBits;
    softBits.reserve(bytes.size() * 8); // 每字节拆成8bit

    for (auto b : bytes) {
        for (int i = 7; i >= 0; --i) {  // 高位到低位
            bool bit = (b >> i) & 0x01;
            float soft = bit ? 1.0f : -1.0f; // 映射到 [-1,1]
            softBits.push_back(soft);
        }
    }
    //  LDPC Decoding
    std::vector<unsigned char> decodedBits = (*this.*ldpcDecode)(softBits);

    if (d_pack) {
        uint8_t* decoded_u8 = (uint8_t*)malloc(sizeof(uint8_t) * decodedBits.size() / 8);
        for (int i = 0; i < decodedBits.size() / 8; i++) {
            decoded_u8[i] = 0;
            for (int j = 0; j < 8; j++) {
                decoded_u8[i] |= decodedBits[i * 8 + j] ? (0x80 >> j) : 0;
            }
        }
        // Publishing data
        pmt::pmt_t pdu(
            pmt::cons(pmt::PMT_NIL, pmt::make_blob(decoded_u8, decodedBits.size() / 8)));
        message_port_pub(d_out_port, pdu);
        free(decoded_u8);
    } else {
        // Publishing data
        pmt::pmt_t pdu(pmt::cons(pmt::PMT_NIL,
                                 pmt::make_blob(decodedBits.data(), decodedBits.size())));
        message_port_pub(d_out_port, pdu);
    }
    decodedBits.clear();
    softBits.clear();
}


/*
 * Our virtual destructor.
 */
ldpc_decoder_impl::~ldpc_decoder_impl() {}

void ldpc_decoder_impl::forecast(int noutput_items, gr_vector_int& ninput_items_required)
{
}

int ldpc_decoder_impl::general_work(int noutput_items,
                                    gr_vector_int& ninput_items,
                                    gr_vector_const_void_star& input_items,
                                    gr_vector_void_star& output_items)
{

    consume_each(noutput_items);

    // Tell runtime system how many output items we produced.
    return noutput_items;
}

} /* namespace hsdec */
} /* namespace gr */
