/* -*- c++ -*- */
/*
 * Copyright 2025 gr-hsdec author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_HSDEC_SEQUENTIAL_MUX_H
#define INCLUDED_HSDEC_SEQUENTIAL_MUX_H

#include <gnuradio/block.h>
#include <gnuradio/hsdec/api.h>

namespace gr {
namespace hsdec {

/*!
 * \brief <+description of block+>
 * \ingroup hsdec
 *
 */
class HSDEC_API sequential_mux : virtual public gr::block
{
public:
    typedef std::shared_ptr<sequential_mux> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of hsdec::sequential_mux.
     *
     * To avoid accidental use of raw pointers, hsdec::sequential_mux's
     * constructor is in a private implementation
     * class. hsdec::sequential_mux::make is the public interface for
     * creating new instances.
     */
    static sptr make(int n);
    virtual void set_n(int n) = 0;
};

} // namespace hsdec
} // namespace gr

#endif /* INCLUDED_HSDEC_SEQUENTIAL_MUX_H */
