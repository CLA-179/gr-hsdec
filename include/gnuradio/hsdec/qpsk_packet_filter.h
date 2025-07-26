/* -*- c++ -*- */
/*
 * Copyright 2025 gr-hsdec author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_HSDEC_QPSK_PACKET_FILTER_H
#define INCLUDED_HSDEC_QPSK_PACKET_FILTER_H

#include <gnuradio/block.h>
#include <gnuradio/hsdec/api.h>

namespace gr {
namespace hsdec {

/*!
 * \brief <+description of block+>
 * \ingroup hsdec
 *
 */
class HSDEC_API qpsk_packet_filter : virtual public gr::block
{
public:
    typedef std::shared_ptr<qpsk_packet_filter> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of hsdec::qpsk_packet_filter.
     *
     * To avoid accidental use of raw pointers, hsdec::qpsk_packet_filter's
     * constructor is in a private implementation
     * class. hsdec::qpsk_packet_filter::make is the public interface for
     * creating new instances.
     */
    static sptr make(int chunk_len, int sps);
};

} // namespace hsdec
} // namespace gr

#endif /* INCLUDED_HSDEC_QPSK_PACKET_FILTER_H */
