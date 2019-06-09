# - Try to find indi Universal Library
# Once done this will define
#
#  INDI_FOUND - system has indi
#  INDI_INCLUDE_DIR - the indi include directory
#  INDI_LIBRARIES - Link these to use indi

# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

if (INDI_INCLUDE_DIR AND INDI_LIBRARIES)

  # in cache already
  set(INDI_FOUND TRUE)
  message(STATUS "Found libindi: ${INDI_LIBRARIES}")

else (INDI_INCLUDE_DIR AND INDI_LIBRARIES)

  find_path(INDI_INCLUDE_DIR indiapi.h
    PATH_SUFFIXES libindi
    ${_obIncDir}
    ${GNUWIN32_DIR}/include
  )

  find_library(INDI_LIBRARIES NAMES indidriver
    PATHS
    ${_obLinkDir}
    ${GNUWIN32_DIR}/lib
  )

  if(INDI_INCLUDE_DIR AND INDI_LIBRARIES)
    set(INDI_FOUND TRUE)
    set(INDI_DATA_DIR ${GNUWIN32_DIR}/share/indi)
  else (INDI_INCLUDE_DIR AND INDI_LIBRARIES)
    set(INDI_FOUND FALSE)
  endif(INDI_INCLUDE_DIR AND INDI_LIBRARIES)


  if (INDI_FOUND)
    if (NOT indi_FIND_QUIETLY)
      message(STATUS "Found indi: ${INDI_LIBRARIES}")
    endif (NOT indi_FIND_QUIETLY)
  else (INDI_FOUND)
    if (indi_FIND_REQUIRED)
      message(FATAL_ERROR "indi not found. Please install indi Library http://www.indilib.org")
    endif (indi_FIND_REQUIRED)
  endif (INDI_FOUND)

  mark_as_advanced(INDI_INCLUDE_DIR INDI_LIBRARIES)
  
endif (INDI_INCLUDE_DIR AND INDI_LIBRARIES)
