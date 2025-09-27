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

namespace gr {
namespace hsdec {


using output_type = char;
send_pack::sptr send_pack::make() { return gnuradio::make_block_sptr<send_pack_impl>(); }


/*
 * The private constructor
 */
send_pack_impl::send_pack_impl()
    : gr::sync_block("send_pack",
                     gr::io_signature::make(0, 0, 0),
                     gr::io_signature::make(
                         1 /* min outputs */, 1 /*max outputs */, sizeof(output_type)))
{
    d_in_port = pmt::mp("in");
    message_port_register_in(d_in_port);
    set_msg_handler(d_in_port, [this](const pmt::pmt_t& msg) { pmt_in_callback(msg); });
    set_min_output_buffer(1024*4);
}

void send_pack_impl::pmt_in_callback(pmt::pmt_t msg)
{
    pmt::pmt_t meta(pmt::car(msg));
    pmt::pmt_t bytes(pmt::cdr(msg));

    size_t msg_len, n_path;
    const uint8_t* bytes_in = pmt::u8vector_elements(bytes, msg_len);
    // memset(data_frame, 0xAA, MAX_FEC_LENGTH);
    int i = 0;

    while (msg_len >= 1024) {
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

    for (int i = 0; i < maxpackes; i++) {
        if (!fifo.empty() && noutput_items - out_items >= 1024) {
            uint8_t* data = fifo.front();
            fifo.pop_front();
            memcpy(out + i * 1024, data, 1024);
            free(data);
        }
        else
        {
            out[i * 1024 + 0] = 0x1A;
            out[i * 1024 + 1] = 0xCF;
            out[i * 1024 + 2] = 0xFC;
            out[i * 1024 + 3] = 0x1D;
            
            memset(out + i * 1024 + 4, 0xAA, 1020);

        }
        out_items += 1024;
    }


    return out_items;
}

} /* namespace hsdec */
} /* namespace gr */
