/* -*- c++ -*- */
/*
 * Copyright 2025 gr-hsdec author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_HSDEC_LDPC_DECODER_H
#define INCLUDED_HSDEC_LDPC_DECODER_H

#include <gnuradio/block.h>
#include <gnuradio/hsdec/api.h>

namespace gr {
namespace hsdec {

/*!
 * \brief <+description of block+>
 * \ingroup hsdec
 *
 */
class HSDEC_API ldpc_decoder : virtual public gr::block
{
public:
    typedef std::shared_ptr<ldpc_decoder> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of hsdec::ldpc_decoder.
     *
     * To avoid accidental use of raw pointers, hsdec::ldpc_decoder's
     * constructor is in a private implementation
     * class. hsdec::ldpc_decoder::make is the public interface for
     * creating new instances.
     */
    static sptr make(std::string alist_file, int code_type,int iterations, float sigma, int update_sigma,int pack);
};

} // namespace hsdec
} // namespace gr

#endif /* INCLUDED_HSDEC_LDPC_DECODER_H */
