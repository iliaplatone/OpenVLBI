# - Try to find libopendsp
# Once done this will define
#
#  DSP_FOUND - system has libopendsp
#  DSP_INCLUDE_DIRS - the libopendsp include directories
#  DSP_LIBRARIES - Link these to use libopendsp
#  DSP_DEFINITIONS - Compiler switches required for using libopendsp
#
#  DSP_HAS_DSP_VERSION - defined when libopendsp has dsp_get_version()

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

find_path(DSP_INCLUDE_DIR
  NAMES
    dsp.h
  HINTS
    ${DSP_INCLUDE_DIRS}
  PATH_SUFFIXES
    OpenVLBI
)

find_library(DSP_LIBRARIES
  NAMES
    opendsp
  HINTS
    ${DSP_LIBRARY_DIRS}
)

set(DSP_INCLUDE_DIRS ${DSP_INCLUDE_DIR})
set(DSP_LIBRARIES ${DSP_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(DSP
  FOUND_VAR
    DSP_FOUND
  REQUIRED_VARS
    DSP_LIBRARIES
    DSP_INCLUDE_DIRS
  VERSION_VAR
    DSP_VERSION
)

mark_as_advanced(DSP_INCLUDE_DIRS DSP_LIBRARIES)

if(DSP_FOUND)
  include(CheckCXXSourceCompiles)
  include(CMakePushCheckState)
  cmake_push_check_state(RESET)
  set(CMAKE_REQUIRED_INCLUDES ${DSP_INCLUDE_DIRS})
  set(CMAKE_REQUIRED_LIBRARIES ${DSP_LIBRARIES})
  check_cxx_source_compiles("#include <dsp.h>
    int main() { dsp_get_version(); return 0; }" DSP_HAS_DSP_VERSION)
  cmake_pop_check_state()
endif()
