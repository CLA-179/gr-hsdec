# Copyright 2011,2012,2016,2018,2019 Free Software Foundation, Inc.
#
# This file was generated by gr_modtool, a tool from the GNU Radio framework
# This file is a part of gr-hsdec
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Setup library
########################################################################
include(GrPlatform) #define LIB_SUFFIX

list(APPEND hsdec_sources
    tail_impl.cc
    sequential_mux_impl.cc
    frame_sync_impl.cc
    packet_filter_impl.cc
    qpsk_frame_sync_impl.cc
    qpsk_packet_filter_impl.cc
)

set(hsdec_sources "${hsdec_sources}" PARENT_SCOPE)
if(NOT hsdec_sources)
    MESSAGE(STATUS "No C++ sources... skipping lib/")
    return()
endif(NOT hsdec_sources)

add_library(gnuradio-hsdec SHARED ${hsdec_sources})
target_link_libraries(gnuradio-hsdec gnuradio::gnuradio-runtime)
target_include_directories(gnuradio-hsdec
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    PUBLIC $<INSTALL_INTERFACE:include>
  )
set_target_properties(gnuradio-hsdec PROPERTIES DEFINE_SYMBOL "gnuradio_hsdec_EXPORTS")

if(APPLE)
    set_target_properties(gnuradio-hsdec PROPERTIES
        INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
    )
endif(APPLE)

########################################################################
# Install built library files
########################################################################
include(GrMiscUtils)
GR_LIBRARY_FOO(gnuradio-hsdec)

########################################################################
# Print summary
########################################################################
message(STATUS "Using install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Building for version: ${VERSION} / ${LIBVER}")

########################################################################
# Build and register unit test
########################################################################
include(GrTest)

# If your unit tests require special include paths, add them here
#include_directories()
# List all files that contain Boost.UTF unit tests here
list(APPEND test_hsdec_sources
)
# Anything we need to link to for the unit tests go here
list(APPEND GR_TEST_TARGET_DEPS gnuradio-hsdec)

if(NOT test_hsdec_sources)
    MESSAGE(STATUS "No C++ unit tests... skipping")
    return()
endif(NOT test_hsdec_sources)

foreach(qa_file ${test_hsdec_sources})
    GR_ADD_CPP_TEST("hsdec_${qa_file}"
        ${CMAKE_CURRENT_SOURCE_DIR}/${qa_file}
    )
endforeach(qa_file)
