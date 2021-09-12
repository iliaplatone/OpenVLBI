# - Try to find JSONPARSER
# Once done this will define
#
#  JSONPARSER_FOUND - system has JSONPARSER
#  JSONPARSER_INCLUDE_DIR - the JSONPARSER include directory
#  JSONPARSER_LIBRARIES - Link these to use JSONPARSER

# Copyright (c) 2006, Jasem Mutlaq <mutlaqja@ikarustech.com>
# Based on FindLibfacile by Carsten Niehaus, <cniehaus@gmx.de>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

if (JSONPARSER_INCLUDE_DIR AND JSONPARSER_LIBRARIES)

  # in cache already
  set(JSONPARSER_FOUND TRUE)
  message(STATUS "Found libjsonparser: ${JSONPARSER_LIBRARIES}")

else (JSONPARSER_INCLUDE_DIR AND JSONPARSER_LIBRARIES)

  find_path(JSONPARSER_INCLUDE_DIR json.h
    PATH_SUFFIXES json-parser
    ${_obIncDir}
    ${GNUWIN32_DIR}/include
  )

  find_library(JSONPARSER_LIBRARIES NAMES jsonparser libjsonparser
    PATHS
    ${_obLinkDir}
    ${GNUWIN32_DIR}/lib
    HINTS ${CMAKE_C_IMPLICIT_LINK_DIRECTORIES}
  )

 set(CMAKE_REQUIRED_INCLUDES ${JSONPARSER_INCLUDE_DIR})
 set(CMAKE_REQUIRED_LIBRARIES ${JSONPARSER_LIBRARIES})

   if(JSONPARSER_INCLUDE_DIR AND JSONPARSER_LIBRARIES)
    set(JSONPARSER_FOUND TRUE)
  else (JSONPARSER_INCLUDE_DIR AND JSONPARSER_LIBRARIES)
    set(JSONPARSER_FOUND FALSE)
  endif(JSONPARSER_INCLUDE_DIR AND JSONPARSER_LIBRARIES)

  if (JSONPARSER_FOUND)
    if (NOT JSONParser_FIND_QUIETLY)
      message(STATUS "Found JSONPARSER: ${JSONPARSER_LIBRARIES}")
    endif (NOT JSONParser_FIND_QUIETLY)
  else (JSONPARSER_FOUND)
    if (JSONParser_FIND_REQUIRED)
      message(FATAL_ERROR "libjsonparser not found. Please install libjsonparser development package.")
    endif (JSONParser_FIND_REQUIRED)
  endif (JSONPARSER_FOUND)

  mark_as_advanced(JSONPARSER_INCLUDE_DIR JSONPARSER_LIBRARIES)
  
endif (JSONPARSER_INCLUDE_DIR AND JSONPARSER_LIBRARIES)
