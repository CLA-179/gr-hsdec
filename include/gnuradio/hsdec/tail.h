/* -*- c++ -*- */
/*
 * Copyright 2025 gr-hsdec author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_HSDEC_TAIL_H
#define INCLUDED_HSDEC_TAIL_H

#include <gnuradio/hsdec/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace hsdec {

    /*!
     * \brief <+description of block+>
     * \ingroup hsdec
     *
     */
    class HSDEC_API tail : virtual public gr::block
    {
     public:
      typedef std::shared_ptr<tail> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of hsdec::tail.
       *
       * To avoid accidental use of raw pointers, hsdec::tail's
       * constructor is in a private implementation
       * class. hsdec::tail::make is the public interface for
       * creating new instances.
       */
      static sptr make(int n, int m);
    };

  } // namespace hsdec
} // namespace gr

#endif /* INCLUDED_HSDEC_TAIL_H */
