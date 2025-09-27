/* -*- c++ -*- */
/*
 * Copyright 2025 gr-hsdec author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "rs_4_impl.h"
#include <gnuradio/io_signature.h>
#include "fec/rs/rs.h"

namespace gr {
namespace hsdec {


rs_4::sptr rs_4::make() { return gnuradio::make_block_sptr<rs_4_impl>(); }


/*
 * The private constructor
 */
rs_4_impl::rs_4_impl()
    : gr::block("rs_4",
                gr::io_signature::make(
                    0,0,0),
                gr::io_signature::make(
                    0,0,0))
{

    d_in_port = pmt::mp("in");
    message_port_register_in(d_in_port);

    d_out_port = pmt::mp("out");
    message_port_register_out(d_out_port);

    set_msg_handler(d_in_port, [this](const pmt::pmt_t& msg) { pmt_in_callback(msg); });
}

void encoder_rs_4(const uint8_t* data_in, uint8_t* data_out_in)
{
    uint8_t rs_block[4][255];
    // int n[4] = {0,0,0,0};
    const uint8_t* pdata = data_in + 4;

    memcpy(data_out_in, data_in, 4);

    // for (int i = 892 / 4 - 1; i > -1; i --)
    for (int i = 0; i < 892 / 4; i ++)
    {
        rs_block[0][i] = *(pdata ++);
        rs_block[1][i] = *(pdata ++);
        rs_block[2][i] = *(pdata ++);
        rs_block[3][i] = *(pdata ++);
    }

    for (int i = 0; i < 4; i++)
    {
        encode_rs_ccsds(rs_block[i], rs_block[i] + 223, 0);
    }
    uint8_t* data_out = data_out_in + 4;
    int out_index = 0;
    // for (int i = 223 - 1; i > -1; i--)
    for (int i = 0; i < 223; i++)
    {
        data_out[out_index ++] = rs_block[0][i];
        data_out[out_index ++] = rs_block[1][i];
        data_out[out_index ++] = rs_block[2][i];
        data_out[out_index ++] = rs_block[3][i];
    }
    // for (int i = 255 - 1; i > 223 - 1; i--)
    for (int i = 223; i < 255; i++)
    {
        data_out[out_index ++] = rs_block[0][i];
        data_out[out_index ++] = rs_block[1][i];
        data_out[out_index ++] = rs_block[2][i];
        data_out[out_index ++] = rs_block[3][i];
    }
    
    
    
}

unsigned char count_one(unsigned char in)
{
    static unsigned char out[16] = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4};

    int o = out[in & 0x0f];
    o += out[(in >> 4) & 0x0f];

    // printf ("%d\n", o);

    return o;
}

void LOS_ccsds_scramble(unsigned char *data, unsigned short len, unsigned char _x, unsigned char _n)
{
    unsigned char n = _n;
    unsigned char x = _x;
    unsigned char temp = 0;
    unsigned char SC_temp = 0;

    for (unsigned short i = 0; i < len; i++)
    {
        for (unsigned char j = 0; j < 8; j++)
        {
            // printf("%d", x & 0x01);
            SC_temp = (SC_temp << 1) | ((x & 0x80) >> 7);
            temp = (count_one(x & n) & 0x01);
            x = ((x << 1) & 0xFE) | temp;
        }
        // printf("\n%x\n\n", SC_temp);

        data[i] ^= SC_temp;
    }
}

void rs_4_impl::pmt_in_callback(pmt::pmt_t msg)
{
    pmt::pmt_t meta(pmt::car(msg));
    pmt::pmt_t bytes(pmt::cdr(msg));

    size_t msg_len, n_path;
    const uint8_t* bytes_in = pmt::u8vector_elements(bytes, msg_len);
    // memset(data_frame, 0xAA, MAX_FEC_LENGTH);
    int i = 0;

    while (msg_len >= 1024) {

        std::vector<unsigned char> encoded_data;
        encoded_data.resize(1024);
        encoder_rs_4(bytes_in, encoded_data.data());
        LOS_ccsds_scramble(encoded_data.data() + 4, 1020, 0xFF, 0x95); // 加扰（去掉同步头）

        pmt::pmt_t pdu(pmt::cons(pmt::PMT_NIL,
                                 pmt::make_blob(encoded_data.data(), encoded_data.size())));
        message_port_pub(d_out_port, pdu);


        msg_len -= 1024;
        i++;
    }
}

/*
 * Our virtual destructor.
 */
rs_4_impl::~rs_4_impl() {}

void rs_4_impl::forecast(int noutput_items, gr_vector_int& ninput_items_required)
{

    /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
}

int rs_4_impl::general_work(int noutput_items,
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
