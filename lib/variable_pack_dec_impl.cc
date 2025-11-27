/* -*- c++ -*- */
/*
 * Copyright 2025 gr-hsdec author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "variable_pack_dec_impl.h"
#include <gnuradio/io_signature.h>
#include "fec/rs/rs.h"

namespace gr {
namespace hsdec {

using input_type = unsigned char;

variable_pack_dec::sptr variable_pack_dec::make(std::string call, int order)
{
    return gnuradio::make_block_sptr<variable_pack_dec_impl>(call, order);
}


/*
 * The private constructor
 */
variable_pack_dec_impl::variable_pack_dec_impl(std::string call, int order)
    : gr::block("variable_pack_dec",
                gr::io_signature::make(
                    1 /* min inputs */, 1 /* max inputs */, sizeof(input_type)),
                gr::io_signature::make(0, 0, 0))
{
    d_call_pass = call;
    d_order = order;


    d_out_port = pmt::mp("out");
    message_port_register_out(d_out_port);

    printf("Pass Call: %s\n", d_call_pass.c_str());


}

/*
 * Our virtual destructor.
 */
variable_pack_dec_impl::~variable_pack_dec_impl() {}

void variable_pack_dec_impl::forecast(int noutput_items,
                                      gr_vector_int& ninput_items_required)
{
    ninput_items_required[0] = noutput_items;
}

static int count_one32(unsigned int in)
{
    static int out[16] = { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4 };

    int o = out[in & 0x0f];
    o += out[(in >> 4) & 0x0f];
    o += out[(in >> 8) & 0x0f];
    o += out[(in >> 12) & 0x0f];
    o += out[(in >> 16) & 0x0f];
    o += out[(in >> 20) & 0x0f];
    o += out[(in >> 24) & 0x0f];
    o += out[(in >> 28) & 0x0f];

    // printf ("%d\n", o);

    return o;
}

int variable_pack_dec_impl::decoder(std::vector<uint8_t> &data)
{
    int dec_len = data.size() - (data.size() / 255 + 1) * 32;
    std::vector<uint8_t> dec_data(dec_len);
    int i;
    for (i = 0; i < (int)data.size() / 255; i++)
    {
        decode_rs(data.data() + i * 255, NULL, 0, 0);
        memcpy(dec_data.data() + i * 223, data.data() + i * 255, 223);
    }

    decode_rs(data.data() + i * 255, NULL, 0, 255 - (data.size() % 255));
    memcpy(dec_data.data() + i * 223, data.data() + i * 255, (data.size() % 255) - 32);

    // // data.assign(dec_data, dec_data + dec_len);
    data = std::move(dec_data);
    return dec_len;
}

int variable_pack_dec_impl::general_work(int noutput_items,
                                         gr_vector_int& ninput_items,
                                         gr_vector_const_void_star& input_items,
                                         gr_vector_void_star& output_items)
{
    auto in = static_cast<const input_type*>(input_items[0]);

    if (d_order == 2) {
        for (int i = 0; i < ninput_items[0]; ++i) {

            if (d_skip_count > 0) {
                d_skip_count--;
                continue;
            }

            if (d_len == 0 && d_len_index == 0)
            {
                d_sync_flag = false;
                d_data = false;
                d_call = false;
            }
            
            if (d_sync_flag && d_len_index == 0 && !d_call && !d_data)
            {
                d_call = true;
                d_call_index = 0;
                d_data_bit_mask = 0x80;
                printf("recv len %d\n", d_len);

            }
            

            if (!d_sync_flag)
            {
                d_data_reg = ((d_data_reg << 1) | (in[i] & 0x1));

                if (d_data_reg_bits < 32) {
                    d_data_reg_bits++;
                    continue;
                }


                // Full 32 bits in shift register
                uint64_t wrong_bits = (d_data_reg ^ d_head_sync) & d_mask;
                uint64_t nwrong = count_one32(wrong_bits);


                if (nwrong <= d_threshold) {
                    d_flip_2 = false;
                    d_sync_flag = true;
                    d_len_index = 0x8000;
                    d_len = 0;
                    printf("sync\n");
                    continue;
                } else if (nwrong >= 32 - d_threshold) {
                    d_flip_2 = true;
                    d_sync_flag = true;
                    d_len_index = 0x8000;
                    d_len = 0;
                    printf("sync flip\n");
                    continue;
                }
            }
            else if (d_len_index > 0)
            {
                d_len |= in[i] ? (d_flip_2 ? 0 : d_len_index) : (d_flip_2 ? d_len_index : 0);
                d_len_index >>= 1;
            }
            else if (d_call && !d_data)
            {
                call_recv[d_call_index / 8] <<= 1;
                call_recv[d_call_index / 8] |= in[i] ? (d_flip_2 ? 0 : 1) : (d_flip_2 ? 1 : 0);
                
                d_call_index ++;
                if (d_call_index == 6 * 8)
                {
                    call_recv[6] = '\0';
                    printf("recv from %s\n", call_recv);
                    d_data = true;
                    // d_sync_flag = false;
                    d_data_bit_mask = 0x80;
                    // decoded_data.resize(d_len);
                    decoded_data.assign(d_len, 0);
                    d_data_index = 0;
                }
                
            }
            else
            {
                decoded_data.data()[d_data_index] |= in[i] ? (d_flip_2 ? 0 : d_data_bit_mask) : (d_flip_2 ? d_data_bit_mask : 0);
                d_data_bit_mask >>= 1;
                if (d_data_bit_mask == 0)
                {
                    d_data_bit_mask = 0x80;
                    d_data_index ++;
                    if (d_data_index >= d_len)
                    {
                        d_len = 0;
                        d_call = false;
                        d_data = false;
                        d_sync_flag = 0;
                        d_data_index = 0;
                        if (strcmp((char*)call_recv, d_call_pass.c_str()) == 0)
                        {
                            continue;
                        }
                        decoder(decoded_data);
                        pmt::pmt_t pdu(pmt::cons(pmt::PMT_NIL,
                                pmt::make_blob(decoded_data.data(), decoded_data.size())));
                        message_port_pub(d_out_port, pdu);
                    }
                    
                }
                
                
            }
            
        }
    } else {
    }


    consume_each(ninput_items[0]);

    // Tell runtime system how many output items we produced.
    return 0;
}

} /* namespace hsdec */
} /* namespace gr */
