# - Try to find JSON_C Universal Library
# Once done this will define
#
#  JSON_C_FOUND - system has JSON_C
#  JSON_C_INCLUDE_DIR - the JSON_C include directory
#  JSON_C_LIBRARIES - Link these to use JSON_C

# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

if (JSON_C_INCLUDE_DIR AND JSON_C_LIBRARIES)

  # in cache already
  set(JSON_C_FOUND TRUE)
  message(STATUS "Found libJSON_C: ${JSON_C_LIBRARIES}")

else (JSON_C_INCLUDE_DIR AND JSON_C_LIBRARIES)

  find_path(JSON_C_INCLUDE_DIR json.h
    PATH_SUFFIXES json-c
    ${_obIncDir}
    ${GNUWIN32_DIR}/include
  )

  find_library(JSON_C_LIBRARIES NAMES json-c
    PATHS
    ${_obLinkDir}
    ${GNUWIN32_DIR}/lib
  )

  if(JSON_C_INCLUDE_DIR AND JSON_C_LIBRARIES)
    set(JSON_C_FOUND TRUE)
  else (JSON_C_INCLUDE_DIR AND JSON_C_LIBRARIES)
    set(JSON_C_FOUND FALSE)
  endif(JSON_C_INCLUDE_DIR AND JSON_C_LIBRARIES)


  if (JSON_C_FOUND)
    if (NOT JSON_C_FIND_QUIETLY)
      message(STATUS "Found JSON_C: ${JSON_C_LIBRARIES}")
    endif (NOT JSON_C_FIND_QUIETLY)
  else (JSON_C_FOUND)
    if (JSON_C_FIND_REQUIRED)
      message(FATAL_ERROR "JSON_C not found. Please install JSON_C Library http://www.JSON_Clib.org")
    endif (JSON_C_FIND_REQUIRED)
  endif (JSON_C_FOUND)

  mark_as_advanced(JSON_C_INCLUDE_DIR JSON_C_LIBRARIES)

endif (JSON_C_INCLUDE_DIR AND JSON_C_LIBRARIES)
