/* -*- c++ -*- */
/*
 * Copyright 2025 gr-hsdec author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_HSDEC_RS_4_H
#define INCLUDED_HSDEC_RS_4_H

#include <gnuradio/block.h>
#include <gnuradio/hsdec/api.h>

namespace gr {
namespace hsdec {

/*!
 * \brief <+description of block+>
 * \ingroup hsdec
 *
 */
class HSDEC_API rs_4 : virtual public gr::block
{
public:
    typedef std::shared_ptr<rs_4> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of hsdec::rs_4.
     *
     * To avoid accidental use of raw pointers, hsdec::rs_4's
     * constructor is in a private implementation
     * class. hsdec::rs_4::make is the public interface for
     * creating new instances.
     */
    static sptr make();
};

} // namespace hsdec
} // namespace gr

#endif /* INCLUDED_HSDEC_RS_4_H */
