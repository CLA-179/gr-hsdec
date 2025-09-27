/* -*- c++ -*- */
/*
 * Copyright 2025 gr-hsdec author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "squelch_new_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace hsdec {

// #pragma message("set the following appropriately and remove this warning")
using input_type = gr_complex;
// #pragma message("set the following appropriately and remove this warning")
using output_type = gr_complex;
squelch_new::sptr squelch_new::make(float thre) { return gnuradio::make_block_sptr<squelch_new_impl>(thre); }


/*
 * The private constructor
 */
squelch_new_impl::squelch_new_impl(float thre)
    : gr::block("squelch_new",
                gr::io_signature::make(
                    1 /* min inputs */, 1 /* max inputs */, sizeof(input_type)),
                gr::io_signature::make(
                    1 /* min outputs */, 1 /*max outputs */, sizeof(output_type)))
{
    d_thre = thre;
}

/*
 * Our virtual destructor.
 */
squelch_new_impl::~squelch_new_impl() {}

void squelch_new_impl::forecast(int noutput_items, gr_vector_int& ninput_items_required)
{
    ninput_items_required[0] = noutput_items;
}

int squelch_new_impl::general_work(int noutput_items,
                               gr_vector_int& ninput_items,
                               gr_vector_const_void_star& input_items,
                               gr_vector_void_star& output_items)
{
    auto in = static_cast<const input_type*>(input_items[0]);
    auto out = static_cast<output_type*>(output_items[0]);

    int out_count = 0;

    for (int i = 0; i < ninput_items[0]; i++)
    {
        if (in[i].imag() < d_thre && in[i].real() < d_thre)
        {
            if (zero_count < 1000)
            {
                zero_count++;
            }
        }
        else 
        {
            zero_count = 0;
        }
        
        if (zero_count < 1000)
        {
            out[out_count ++] = in[i];
        }
        

        
    }
    

    consume_each(ninput_items[0]);

    // Tell runtime system how many output items we produced.
    return out_count;
}

} /* namespace hsdec */
} /* namespace gr */
