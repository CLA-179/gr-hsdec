/* -*- c++ -*- */
/*
 * Copyright 2025 gr-hsdec author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_HSDEC_FRAME_SYNC_H
#define INCLUDED_HSDEC_FRAME_SYNC_H

#include <gnuradio/block.h>
#include <gnuradio/hsdec/api.h>

namespace gr {
namespace hsdec {

/*!
 * \brief <+description of block+>
 * \ingroup hsdec
 *
 */
class HSDEC_API frame_sync : virtual public gr::block
{
public:
    typedef std::shared_ptr<frame_sync> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of hsdec::frame_sync.
     *
     * To avoid accidental use of raw pointers, hsdec::frame_sync's
     * constructor is in a private implementation
     * class. hsdec::frame_sync::make is the public interface for
     * creating new instances.
     */
    static sptr make(uint32_t code, int len, int block_size);
};

} // namespace hsdec
} // namespace gr

#endif /* INCLUDED_HSDEC_FRAME_SYNC_H */
