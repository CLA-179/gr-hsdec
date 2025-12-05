/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <pybind11/pybind11.h>

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/arrayobject.h>

namespace py = pybind11;

// Headers for binding functions
/**************************************/
// The following comment block is used for
// gr_modtool to insert function prototypes
// Please do not delete
/**************************************/
// BINDING_FUNCTION_PROTOTYPES(
    void bind_tail(py::module& m);
    void bind_sequential_mux(py::module& m);
    void bind_frame_sync(py::module& m);
    void bind_packet_filter(py::module& m);
    void bind_qpsk_frame_sync(py::module& m);
    void bind_qpsk_packet_filter(py::module& m);
    void bind_squelch_new(py::module& m);
    void bind_ldpc_decoder(py::module& m);
    void bind_send_pack(py::module& m);
    void bind_rs_4(py::module& m);
    void bind_variable_pack_enc(py::module& m);
    void bind_variable_pack_dec(py::module& m);
// ) END BINDING_FUNCTION_PROTOTYPES


// We need this hack because import_array() returns NULL
// for newer Python versions.
// This function is also necessary because it ensures access to the C API
// and removes a warning.
void* init_numpy()
{
    import_array();
    return NULL;
}

PYBIND11_MODULE(hsdec_python, m)
{
    // Initialize the numpy C API
    // (otherwise we will see segmentation faults)
    init_numpy();

    // Allow access to base block methods
    py::module::import("gnuradio.gr");

    /**************************************/
    // The following comment block is used for
    // gr_modtool to insert binding function calls
    // Please do not delete
    /**************************************/
    // BINDING_FUNCTION_CALLS(
    bind_tail(m);
    bind_sequential_mux(m);
    bind_frame_sync(m);
    bind_packet_filter(m);
    bind_qpsk_frame_sync(m);
    bind_qpsk_packet_filter(m);
    bind_squelch_new(m);
    bind_ldpc_decoder(m);
    bind_send_pack(m);
    bind_rs_4(m);
    bind_variable_pack_enc(m);
    bind_variable_pack_dec(m);
    // ) END BINDING_FUNCTION_CALLS
}