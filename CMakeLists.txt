cmake_minimum_required(VERSION 2.6)

PROJECT(OpenVLBI C CXX)
LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/")

include(GNUInstallDirs)
include(ExternalProject)

find_package(CFITSIO REQUIRED)
find_package(ZLIB REQUIRED)
find_package(FFTW3 REQUIRED)
find_package(Threads REQUIRED)
option(WITH_INDI_CLIENT "Add INDI client for OpenVLBI" ON)
option(WITH_DUMMY_CLIENT "Add dummy client for OpenVLBI" ON)

set (VLBI_VERSION_MAJOR 1)
set (VLBI_VERSION_MINOR 10)
set (VLBI_VERSION_STRING "${VLBI_VERSION_MAJOR}.${VLBI_VERSION_MINOR}.0")

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/")
LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake_modules/")

add_definitions(-Wall -O3 -g -fstack-protector -Wno-unused-parameter)

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/README.md" README )
string(REPLACE "[![Build Status](https://travis-ci.org/iliaplatone/OpenVLBI.svg?branch=master)](https://travis-ci.org/iliaplatone/OpenVLBI)" "" README "${README}")
string(REGEX REPLACE "\n\# \([a-z:A-Z:0-9]*\)" "\n\n\\page page_\1 \1" README "${README}" )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/dsp/dsp.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/dsp.h )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/vlbi/vlbi.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/vlbi.h )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.cmake ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile )

include_directories( ${CMAKE_CURRENT_BINARY_DIR})
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/vlbi)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/dsp)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR})

include(CMakeCommon)

############# VLBI ###############
set(vlbi_CXX_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/vlbi/collection.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vlbi/nodecollection.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vlbi/baselinecollection.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vlbi/node.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vlbi/baseline.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vlbi/stream.cpp
    )

set(vlbi_C_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/vlbi/time.c
    ${CMAKE_CURRENT_SOURCE_DIR}/vlbi/base64.c
    ${CMAKE_CURRENT_SOURCE_DIR}/vlbi/matrix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/vlbi/astro.c
    )

SET(dsp_C_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/dsp/convert.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dsp/fft.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dsp/filters.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dsp/stats.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dsp/buffer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dsp/convolution.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dsp/signals.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dsp/stream.c
    )

execute_process (COMMAND doxygen ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_library(openvlbi SHARED ${dsp_C_SRC} ${vlbi_C_SRCS} ${vlbi_CXX_SRCS})
set_target_properties(openvlbi PROPERTIES VERSION ${VLBI_VERSION_STRING} SOVERSION ${VLBI_VERSION_MAJOR})
target_link_libraries(openvlbi ${FFTW3_LIBRARIES} ${M_LIB} ${ZLIB_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/vlbi.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include/OpenVLBI)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/dsp.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include/OpenVLBI)
install(TARGETS openvlbi LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

if(WITH_INDI_CLIENT)
find_package(INDI REQUIRED)
find_package(INDICLIENT REQUIRED)
if(INDI_LIBRARIES AND INDICLIENT_LIBRARIES)
include_directories( ${INDI_INCLUDE_DIR})
add_executable(vlbi_client_indi ${CMAKE_CURRENT_SOURCE_DIR}/vlbi_client_indi.cpp ${CMAKE_CURRENT_SOURCE_DIR}/vlbi_client.cpp ${CMAKE_CURRENT_SOURCE_DIR}/vlbi/instancecollection.cpp)
target_link_libraries(vlbi_client_indi openvlbi ${INDICLIENT_LIBRARIES} ${CFITSIO_LIBRARIES} ${M_LIB} ${ZLIB_LIBRARY})
install(TARGETS vlbi_client_indi RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
endif(INDI_LIBRARIES AND INDICLIENT_LIBRARIES)
endif(WITH_INDI_CLIENT)

if(WITH_DUMMY_CLIENT)
add_executable(vlbi_client_dummy ${CMAKE_CURRENT_SOURCE_DIR}/vlbi_client.cpp ${CMAKE_CURRENT_SOURCE_DIR}/vlbi_client_dummy.cpp ${CMAKE_CURRENT_SOURCE_DIR}/vlbi/instancecollection.cpp)
target_link_libraries(vlbi_client_dummy openvlbi ${M_LIB} ${ZLIB_LIBRARY})
install(TARGETS vlbi_client_dummy RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
endif(WITH_DUMMY_CLIENT)

if(WITH_JSON_CLIENT)
add_executable(vlbi_client_json ${CMAKE_CURRENT_SOURCE_DIR}/vlbi_client.cpp ${CMAKE_CURRENT_SOURCE_DIR}/vlbi_client_json.cpp ${CMAKE_CURRENT_SOURCE_DIR}/vlbi/instancecollection.cpp)
target_link_libraries(vlbi_client_json openvlbi)
install(TARGETS vlbi_client_json RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
endif(WITH_JSON_CLIENT)
