find_package(PkgConfig)

PKG_CHECK_MODULES(PC_GR_HSDEC gnuradio-hsdec)

FIND_PATH(
    GR_HSDEC_INCLUDE_DIRS
    NAMES gnuradio/hsdec/api.h
    HINTS $ENV{HSDEC_DIR}/include
        ${PC_HSDEC_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    GR_HSDEC_LIBRARIES
    NAMES gnuradio-hsdec
    HINTS $ENV{HSDEC_DIR}/lib
        ${PC_HSDEC_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          )

include("${CMAKE_CURRENT_LIST_DIR}/gnuradio-hsdecTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GR_HSDEC DEFAULT_MSG GR_HSDEC_LIBRARIES GR_HSDEC_INCLUDE_DIRS)
MARK_AS_ADVANCED(GR_HSDEC_LIBRARIES GR_HSDEC_INCLUDE_DIRS)
