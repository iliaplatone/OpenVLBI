# - Try to find Digital Signal Processing for Astronomical Usage
# Once done this will define
#
#  INDICLIENT_FOUND - system has INDICLIENT
#  INDICLIENT_INCLUDE_DIR - the INDICLIENT include directory
#  INDICLIENT_LIBRARIES - Link these to use INDICLIENT

# Copyright (c) 2008, Jasem Mutlaq <mutlaqja@ikarustech.com>
# Based on FindLibfacile by Carsten Niehaus, <cniehaus@gmx.de>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

if (INDICLIENT_INCLUDE_DIR AND INDICLIENT_LIBRARIES)

  # in cache already
  set(INDICLIENT_FOUND TRUE)
  message(STATUS "Found libindiclient: ${INDICLIENT_LIBRARIES}")

else (INDICLIENT_INCLUDE_DIR AND INDICLIENT_LIBRARIES)

  find_path(INDICLIENT_INCLUDE_DIR baseclient.h
    PATH_SUFFIXES indi libindi
    ${_obIncDir}
    ${GNUWIN32_DIR}/include
  )

  find_library(INDICLIENT_LIBRARIES NAMES indiclient
    PATHS
    ${_obLinkDir}
    ${GNUWIN32_DIR}/lib
  )

  if(INDICLIENT_INCLUDE_DIR AND INDICLIENT_LIBRARIES)
    set(INDICLIENT_FOUND TRUE)
  else (INDICLIENT_INCLUDE_DIR AND INDICLIENT_LIBRARIES)
    set(INDICLIENT_FOUND FALSE)
  endif(INDICLIENT_INCLUDE_DIR AND INDICLIENT_LIBRARIES)


  if (INDICLIENT_FOUND)
    if (NOT INDICLIENT_FIND_QUIETLY)
      message(STATUS "Found INDICLIENT: ${INDICLIENT_LIBRARIES}")
    endif (NOT INDICLIENT_FIND_QUIETLY)
  else (INDICLIENT_FOUND)
    if (INDICLIENT_FIND_REQUIRED)
      message(STATUS "INDICLIENT not found. Please install libindi-dev. http://www.indilib.org")
    endif (INDICLIENT_FIND_REQUIRED)
  endif (INDICLIENT_FOUND)

  mark_as_advanced(INDICLIENT_INCLUDE_DIR INDICLIENT_LIBRARIES)
  
endif (INDICLIENT_INCLUDE_DIR AND INDICLIENT_LIBRARIES)
