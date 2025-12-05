/* -*- c++ -*- */
/*
 * Copyright 2025 gr-hsdec author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "variable_pack_enc_impl.h"
#include <gnuradio/io_signature.h>


#include "fec/rs/rs.h"



namespace gr {
namespace hsdec {

#pragma message("set the following appropriately and remove this warning")
using output_type = unsigned char;
variable_pack_enc::sptr variable_pack_enc::make(std::string call, int preamble_len)
{
    return gnuradio::make_block_sptr<variable_pack_enc_impl>(call, preamble_len);
}


/*
 * The private constructor
 */
variable_pack_enc_impl::variable_pack_enc_impl(std::string call, int preamble_len)
    : gr::block("variable_pack_enc",
                gr::io_signature::make(0, 0, 0),
                gr::io_signature::make(
                    1 /* min outputs */, 1 /*max outputs */, sizeof(output_type)))
{

    d_call = call;
    d_in_port = pmt::mp("in");
    d_pre_len = preamble_len;
    message_port_register_in(d_in_port);
    set_msg_handler(d_in_port, [this](const pmt::pmt_t& msg) { pmt_in_callback(msg); });

}

/*
 * Our virtual destructor.
 */
variable_pack_enc_impl::~variable_pack_enc_impl() {}

void variable_pack_enc_impl::forecast(int noutput_items,
                                      gr_vector_int& ninput_items_required)
{

}

void variable_pack_enc_impl::pmt_in_callback(pmt::pmt_t msg) 
{
    pmt::pmt_t meta(pmt::car(msg));
    pmt::pmt_t bytes(pmt::cdr(msg));
    
    static int count = 0;
    size_t msg_len, n_path;
    const uint8_t* bytes_in = pmt::u8vector_elements(bytes, msg_len);
    int pack_len = HEAD_LEN + msg_len + (32 * (msg_len / 223 + 1));
    
    // uint8_t* pack->p_data = (uint8_t *)malloc(msg_len);
    PACK_t* pack = (PACK_t*)malloc(sizeof(PACK_t));
    pack->p_data = (uint8_t *)malloc(pack_len);

    pack->p_data[0] = (d_head_sync & 0xFF000000) >> 24;
    pack->p_data[1] = (d_head_sync & 0x00FF0000) >> 16;
    pack->p_data[2] = (d_head_sync & 0x0000FF00) >> 8;
    pack->p_data[3] = (d_head_sync & 0x000000FF) >> 0;

    uint16_t data_len = pack_len - HEAD_LEN;
    pack->p_data[4] = (data_len & 0xFF00) >> 8;
    pack->p_data[5] = (data_len & 0x00FF) >> 0;
    pack->len = pack_len;

    printf("msg_len = %d, pack_len = %d, data_len = %d\n", msg_len, pack_len, data_len);

    pack->p_data[6] = d_call.data()[0];
    pack->p_data[7] = d_call.data()[1];
    pack->p_data[8] = d_call.data()[2];
    pack->p_data[9] = d_call.data()[3];
    pack->p_data[10] = d_call.data()[4];
    pack->p_data[11] = d_call.data()[5];

    

    int i = 0;
    for (i = 0; i < msg_len / 223; i++)
    {
        memcpy(pack->p_data + HEAD_LEN + i * 255, bytes_in + i * 223, 223);
        encode_rs(pack->p_data + HEAD_LEN + i * 255, pack->p_data + HEAD_LEN + i * 255 + 223, 0);

    }
    
    memcpy(pack->p_data + HEAD_LEN + i * 255, bytes_in + i * 223, (msg_len % 223));
    encode_rs(pack->p_data + HEAD_LEN + i * 255, pack->p_data + HEAD_LEN + i * 255 + (msg_len % 223), 223 - (msg_len % 223));

    // printf("recieve %d bytes\n", pack_len);
    fifo.push_back(pack);

}

int variable_pack_enc_impl::general_work(int noutput_items,
                                         gr_vector_int& ninput_items,
                                         gr_vector_const_void_star& input_items,
                                         gr_vector_void_star& output_items)
{
    auto out = static_cast<output_type*>(output_items[0]);

    int send_bytes = 0;
    // static int send_index = 0;
    // static bool sending_head = false;
    while (!fifo.empty())
    {
        if (!d_sending)
        {
            d_sending = true;
            if (noutput_items < d_pre_len / 8)
            {
                return 0;
            }
            else 
            {
                for (int i = 0; i < d_pre_len / 8; i++)
                {
                    out[send_bytes ++] = 0xAA;
                }
                
            }
            
        }

        PACK_t* pack = fifo.front();

        if (noutput_items - send_bytes < pack->len)
        {
            return send_bytes;
        }
        
        fifo.pop_front();

        for (int i = 0; i < pack->len; i++)
        {
            out[send_bytes ++] = pack->p_data[i];
            // printf (" %02X", pack->p_data[i]);
        }
        free(pack->p_data);
        free(pack);
        // printf("send %d bytes\n", send_bytes);
        
    }

    d_sending = false;
    // Tell runtime system how many output items we produced.
    return send_bytes;
}

} /* namespace hsdec */
} /* namespace gr */
