/* -*- c++ -*- */
/*
 * Copyright 2025 gr-hsdec author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_HSDEC_VARIABLE_PACK_DEC_H
#define INCLUDED_HSDEC_VARIABLE_PACK_DEC_H

#include <gnuradio/hsdec/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace hsdec {

    /*!
     * \brief <+description of block+>
     * \ingroup hsdec
     *
     */
    class HSDEC_API variable_pack_dec : virtual public gr::block
    {
     public:
      typedef std::shared_ptr<variable_pack_dec> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of hsdec::variable_pack_dec.
       *
       * To avoid accidental use of raw pointers, hsdec::variable_pack_dec's
       * constructor is in a private implementation
       * class. hsdec::variable_pack_dec::make is the public interface for
       * creating new instances.
       */
      static sptr make(std::string call, int order);
    };

  } // namespace hsdec
} // namespace gr

#endif /* INCLUDED_HSDEC_VARIABLE_PACK_DEC_H */
