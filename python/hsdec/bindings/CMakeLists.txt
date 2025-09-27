# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Check if there is C++ code at all
########################################################################
if(NOT hsdec_sources)
    MESSAGE(STATUS "No C++ sources... skipping python bindings")
    return()
endif(NOT hsdec_sources)

########################################################################
# Check for pygccxml
########################################################################
GR_PYTHON_CHECK_MODULE_RAW(
    "pygccxml"
    "import pygccxml"
    PYGCCXML_FOUND
    )

include(GrPybind)

########################################################################
# Python Bindings
########################################################################
list(APPEND hsdec_python_files
    tail_python.cc
    sequential_mux_python.cc
    frame_sync_python.cc
    packet_filter_python.cc
    qpsk_frame_sync_python.cc
    qpsk_packet_filter_python.cc
    squelch_new_python.cc
    ldpc_decoder_python.cc
    send_pack_python.cc
    rs_4_python.cc python_bindings.cc)

GR_PYBIND_MAKE_OOT(hsdec
   ../../..
   gr::hsdec
   "${hsdec_python_files}")

# copy in bindings .so file for use in QA test module
add_custom_target(
  copy_bindings_for_tests ALL
  COMMAND
    ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/*.so"
    ${CMAKE_BINARY_DIR}/test_modules/gnuradio/hsdec/
  DEPENDS hsdec_python)

install(TARGETS hsdec_python DESTINATION ${GR_PYTHON_DIR}/gnuradio/hsdec COMPONENT pythonapi)
