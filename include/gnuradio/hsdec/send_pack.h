/* -*- c++ -*- */
/*
 * Copyright 2025 gr-hsdec author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_HSDEC_SEND_PACK_H
#define INCLUDED_HSDEC_SEND_PACK_H

#include <gnuradio/hsdec/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace hsdec {

/*!
 * \brief <+description of block+>
 * \ingroup hsdec
 *
 */
class HSDEC_API send_pack : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<send_pack> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of hsdec::send_pack.
     *
     * To avoid accidental use of raw pointers, hsdec::send_pack's
     * constructor is in a private implementation
     * class. hsdec::send_pack::make is the public interface for
     * creating new instances.
     */
    static sptr make();
};

} // namespace hsdec
} // namespace gr

#endif /* INCLUDED_HSDEC_SEND_PACK_H */
