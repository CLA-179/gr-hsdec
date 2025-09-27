/* -*- c++ -*- */
/*
 * Copyright 2025 gr-hsdec author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_HSDEC_SQUELCH_NEW_H
#define INCLUDED_HSDEC_SQUELCH_NEW_H

#include <gnuradio/block.h>
#include <gnuradio/hsdec/api.h>

namespace gr {
namespace hsdec {

/*!
 * \brief <+description of block+>
 * \ingroup hsdec
 *
 */
class HSDEC_API squelch_new : virtual public gr::block
{
public:
    typedef std::shared_ptr<squelch_new> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of hsdec::squelch_new.
     *
     * To avoid accidental use of raw pointers, hsdec::squelch_new's
     * constructor is in a private implementation
     * class. hsdec::squelch_new::make is the public interface for
     * creating new instances.
     */
    static sptr make(float);
};

} // namespace hsdec
} // namespace gr

#endif /* INCLUDED_HSDEC_SQUELCH_NEW_H */
