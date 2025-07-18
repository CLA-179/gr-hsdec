/* -*- c++ -*- */
/*
 * Copyright 2025 gr-hsdec author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/io_signature.h>
#include "tail_impl.h"

namespace gr {
  namespace hsdec {

    // #pragma message("set the following appropriately and remove this warning")
    using input_type = unsigned char;
    // #pragma message("set the following appropriately and remove this warning")
    using output_type = unsigned char;
    tail::sptr
    tail::make(int n, int m)
    {
      return gnuradio::make_block_sptr<tail_impl>(
        n, m
        );
    }


    /*
     * The private constructor
     */
    tail_impl::tail_impl(int _n, int _m)
      : gr::block("tail",
              gr::io_signature::make(1 /* min inputs */, 1 /* max inputs */, sizeof(input_type)),
              gr::io_signature::make(1 /* min outputs */, 1 /*max outputs */, sizeof(output_type)))
    {
      n = _n;
      m = _m;
      buf = (uint8_t*)malloc(m * sizeof(uint8_t));
    }

    /*
     * Our virtual destructor.
     */
    tail_impl::~tail_impl()
    {
      free((void*)buf);
    }

    void
    tail_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
    // #pragma message("implement a forecast that fills in how many items on each input you need to produce noutput_items and remove this warning")
      /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
      // 为输出 noutput_items * keep_len 个字节，需要输入 noutput_items * period 个字节
      ninput_items_required[0] = noutput_items * m / n;
    }

    int
    tail_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const unsigned char* in = (const unsigned char*)input_items[0];
    unsigned char* out = (unsigned char*)output_items[0];

    int input_available = ninput_items[0];
    int produced = 0;
    int consumed = 0;

    // if (!d_started) {
    //     d_offset_start = nitems_read(0);
    //     d_started = true;
    // }

    // 当前输入流的位置
    // uint64_t abs_input_pos = d_offset_start + consumed;

    while ((input_available - consumed) >= m && produced < noutput_items) {
        // 确保从周期边界对齐开始（非常关键）
        // abs_input_pos = m - n + consumed;
        // int phase = abs_input_pos % m;
        // if (phase != 0) {
        //     int skip = m - phase;
        //     consume(skip);
        //     return 0;
        // }

        // 从 in + consumed + skip_len 拷贝 keep_len 字节到输出
        memcpy(out + produced * n,
               in + consumed + m - n,
               n);

        consumed += m;
        produced++;
    }
    

    consume_each(consumed);
    return produced * n;
    }

  } /* namespace hsdec */
} /* namespace gr */
