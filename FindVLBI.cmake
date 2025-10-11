# - Try to find libopenvlbi
# Once done this will define
#
#  VLBI_FOUND - system has libopenvlbi
#  VLBI_INCLUDE_DIRS - the libopenvlbi include directories
#  VLBI_LIBRARIES - Link these to use libopenvlbi
#  VLBI_DEFINITIONS - Compiler switches required for using libopenvlbi
#
#  VLBI_HAS_VLBI_VERSION - defined when libopenvlbi has vlbi_get_version()

#=============================================================================
# Copyright (c) 2022 Ilia Platone <info@iliaplatone.com>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#=============================================================================

find_path(VLBI_INCLUDE_DIR
  NAMES
    vlbi.h
  HINTS
    ${VLBI_INCLUDE_DIRS}
  PATH_SUFFIXES
    OpenVLBI
)

find_library(VLBI_LIBRARIES
  NAMES
    openvlbi
  HINTS
    ${VLBI_LIBRARY_DIRS}
)

set(VLBI_INCLUDE_DIRS ${VLBI_INCLUDE_DIR})
set(VLBI_LIBRARIES ${VLBI_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(VLBI
  FOUND_VAR
    VLBI_FOUND
  REQUIRED_VARS
    VLBI_LIBRARY
    VLBI_INCLUDE_DIR
  VERSION_VAR
    VLBI_VERSION
)

mark_as_advanced(VLBI_INCLUDE_DIRS VLBI_LIBRARIES)

if(VLBI_FOUND)
  include(CheckCXXSourceCompiles)
  include(CMakePushCheckState)
  cmake_push_check_state(RESET)
  set(CMAKE_REQUIRED_INCLUDES ${VLBI_INCLUDE_DIRS})
  set(CMAKE_REQUIRED_LIBRARIES ${VLBI_LIBRARIES})
  check_cxx_source_compiles("#include <vlbi.h>
    int main() { vlbi_get_version(); return 0; }" VLBI_HAS_VLBI_VERSION)
  cmake_pop_check_state()
endif()
