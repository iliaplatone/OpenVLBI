# - Try to find VLBI
# Once done this will define
#
#  VLBI_FOUND - system has VLBI
#  VLBI_INCLUDE_DIR - the VLBI include directory
#  VLBI_LIBRARIES - Link these to use VLBI
#  VLBI_VERSION_STRING - Human readable version number of ahp_xc
#  VLBI_VERSION_MAJOR  - Major version number of ahp_xc
#  VLBI_VERSION_MINOR  - Minor version number of ahp_xc

# Copyright (c) 2017, Ilia Platone, <info@iliaplatone.com>
# Based on FindLibfacile by Carsten Niehaus, <cniehaus@gmx.de>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

if (VLBI_LIBRARIES)

  # in cache already
  set(VLBI_FOUND TRUE)
  message(STATUS "Found VLBI: ${VLBI_LIBRARIES}")


else (VLBI_LIBRARIES)

  find_library(VLBI_LIBRARIES NAMES openvlbi
    PATHS
    ${_obLinkDir}
    ${GNUWIN32_DIR}/lib
    /usr/local/lib
  )

  if(VLBI_LIBRARIES)
    set(VLBI_FOUND TRUE)
  else (VLBI_LIBRARIES)
    set(VLBI_FOUND FALSE)
  endif(VLBI_LIBRARIES)


  if (VLBI_FOUND)
    if (NOT VLBI_FIND_QUIETLY)
      message(STATUS "Found VLBI: ${VLBI_LIBRARIES}")
    endif (NOT VLBI_FIND_QUIETLY)
  else (VLBI_FOUND)
    if (VLBI_FIND_REQUIRED)
      message(FATAL_ERROR "VLBI not found. Please install libahp_xc-dev")
    endif (VLBI_FIND_REQUIRED)
  endif (VLBI_FOUND)

  mark_as_advanced(VLBI_LIBRARIES)
  
endif (VLBI_LIBRARIES)
