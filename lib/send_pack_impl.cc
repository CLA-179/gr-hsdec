/* -*- c++ -*- */
/*
 * Copyright 2025 gr-hsdec author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "send_pack_impl.h"
#include <gnuradio/io_signature.h>
#include <deque>

std::deque<uint8_t*> fifo;

uint8_t empty_frame[1024];

namespace gr {
namespace hsdec {


using output_type = char;
send_pack::sptr send_pack::make(bool scra, int empty) { return gnuradio::make_block_sptr<send_pack_impl>(scra, empty); }

extern void encoder_rs_4(const uint8_t* data_in, uint8_t* data_out_in);
extern void LOS_ccsds_scramble(unsigned char *data, unsigned short len, unsigned char _x, unsigned char _n);

/*
 * The private constructor
 */
send_pack_impl::send_pack_impl(bool scra, int empty)
    : gr::sync_block("send_pack",
                     gr::io_signature::make(0, 0, 0),
                     gr::io_signature::make(
                         1 /* min outputs */, 1 /*max outputs */, sizeof(output_type)))
{
    d_in_port = pmt::mp("in");
    message_port_register_in(d_in_port);
    set_msg_handler(d_in_port, [this](const pmt::pmt_t& msg) { pmt_in_callback(msg); });
    set_min_output_buffer(1024*4);

    uint8_t buf[1024];
    buf[0] = 0x1A;
    buf[1] = 0xCF;
    buf[2] = 0xFC;
    buf[3] = 0x1D;

    if (empty >= 0 && empty < 0x100)
    {
        memset(buf, empty & 0xFF, 1020);
    }
    else
    {
        buf[4] = 0xEA;
        buf[5] = 0xEA;
        
        for (int i = 6; i < 896; i++)
        {
            buf[i] = i & 0xF | 0xF0 & (i * 47);
        }
    }
    
    encoder_rs_4(buf, empty_frame);

    if (scra)
    {
        LOS_ccsds_scramble(empty_frame+4, 1020, 0xFF, 0x95);
    }
    
}

void send_pack_impl::pmt_in_callback(pmt::pmt_t msg)
{
    pmt::pmt_t meta(pmt::car(msg));
    pmt::pmt_t bytes(pmt::cdr(msg));
    
    static int count = 0;
    size_t msg_len, n_path;
    const uint8_t* bytes_in = pmt::u8vector_elements(bytes, msg_len);
    // memset(data_frame, 0xAA, MAX_FEC_LENGTH);
    int i = 0;

    //printf("recv msg len = %d:\n",msg_len);
    //if(msg_len != 1024) printf("warnning !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    while (msg_len >= 1024) {
        //printf("recv msg len = %d, sended pack = %d\n",msg_len,++count);
        msg_len -= 1024;
        uint8_t* data_in = (uint8_t*)malloc(1024 * sizeof(uint8_t));
        memcpy(data_in, bytes_in + i * 1024, 1024);
        fifo.push_back(data_in);
        i++;
    }
}

/*
 * Our virtual destructor.
 */
send_pack_impl::~send_pack_impl() {}

int send_pack_impl::work(int noutput_items,
                         gr_vector_const_void_star& input_items,
                         gr_vector_void_star& output_items)
{
    auto out = static_cast<output_type*>(output_items[0]);

    int out_items = 0;

    int maxpackes = noutput_items / 1024;
    
    static int rfsend_cnt = 0;

    for (int i = 0; i < maxpackes; i++) {
        if (!fifo.empty() && noutput_items - out_items >= 1024) {
            uint8_t* data = fifo.front();
            fifo.pop_front();
            memcpy(out + i * 1024, data, 1024);
            free(data);
            //printf("rf sended pack = %d\n",++rfsend_cnt);
            //printf("rf sended pack head = %02X %02X %02X %02X %02X %02X\n",*(out + i * 1024), *(out + i * 1024 + 1), *(out + i * 1024 + 2), *(out + i * 1024 +3) , *(out + i * 1024 +4), *(out + i * 1024 + 5));
        }
        else
        {
            // out[i * 1024 + 0] = 0x1A;
            // out[i * 1024 + 1] = 0xCF;
            // out[i * 1024 + 2] = 0xFC;
            // out[i * 1024 + 3] = 0x1D;
            // out[i * 1024 + 4] = 0x15;
            // out[i * 1024 + 5] = 0xA2;
            
            // memset(out + i * 1024 + 6, 0xAA, 1020);

            memcpy(out + i * 1024, empty_frame, 1024);
            out[i * 1024 + 0] = 0x1A;
            out[i * 1024 + 1] = 0xCF;
            out[i * 1024 + 2] = 0xFC;
            out[i * 1024 + 3] = 0x1D;
            // memset(out+1024*i+4, 0xEA, 892);

        }
        out_items += 1024;
    }


    return out_items;
}

} /* namespace hsdec */
} /* namespace gr */
