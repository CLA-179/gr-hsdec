/* -*- c++ -*- */
/*
 * Copyright 2025 gr-hsdec author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_HSDEC_TAIL_IMPL_H
#define INCLUDED_HSDEC_TAIL_IMPL_H

#include <gnuradio/hsdec/tail.h>

namespace gr {
  namespace hsdec {

    class tail_impl : public tail
    {
     private:
      // Nothing to declare in this block.
      int n;
      int m;
      bool d_started = false;
      uint64_t d_offset_start = 0;
      uint8_t *buf;


     public:
      tail_impl(int n, int m);
      ~tail_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);

    };

  } // namespace hsdec
} // namespace gr

#endif /* INCLUDED_HSDEC_TAIL_IMPL_H */
