# Copyright 2011 Free Software Foundation, Inc.
#
# This file was generated by gr_modtool, a tool from the GNU Radio framework
# This file is a part of gr-hsdec
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Create the doxygen configuration file
########################################################################
file(TO_NATIVE_PATH ${CMAKE_SOURCE_DIR} top_srcdir)
file(TO_NATIVE_PATH ${CMAKE_BINARY_DIR} top_builddir)
file(TO_NATIVE_PATH ${CMAKE_SOURCE_DIR} abs_top_srcdir)
file(TO_NATIVE_PATH ${CMAKE_BINARY_DIR} abs_top_builddir)

set(HAVE_DOT ${DOXYGEN_DOT_FOUND})
set(enable_html_docs YES)
set(enable_latex_docs NO)
set(enable_mathjax NO)
set(enable_xml_docs YES)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
@ONLY)

set(BUILT_DIRS ${CMAKE_CURRENT_BINARY_DIR}/xml ${CMAKE_CURRENT_BINARY_DIR}/html)

########################################################################
# Make and install doxygen docs
########################################################################
add_custom_command(
    OUTPUT ${BUILT_DIRS}
    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating documentation with doxygen"
)

add_custom_target(doxygen_target ALL DEPENDS ${BUILT_DIRS})

install(DIRECTORY ${BUILT_DIRS} DESTINATION ${GR_PKG_DOC_DIR})
