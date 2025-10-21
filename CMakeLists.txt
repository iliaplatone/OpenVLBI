cmake_minimum_required(VERSION 2.8...4.0)

PROJECT(OpenVLBI C CXX)
LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/")

include(GNUInstallDirs)
include(ExternalProject)

find_package(CFITSIO REQUIRED)
find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)
find_package(FFTW3 REQUIRED)
find_package(Threads REQUIRED)
option(CMAKE_BUILD_RPATH_USE_ORIGIN "rpath issue fix" ON)
option(WITH_DATA "Add OpenVLBI data" On)
option(WITH_DOCS "Add OpenVLBI documentation" On)
option(WITH_DSP "Add OpenDSP libraries" On)
option(WITH_VLBI "Add OpenVLBI libraries" On)
option(WITH_VLBI_SERVER "Add OpenVLBI servers" On)
option(WITH_INDI_SERVER "Add INDI server for OpenVLBI" On)
option(WITH_DUMMY_SERVER "Add dummy server for OpenVLBI" On)
option(WITH_JSON_SERVER "Add JSON server for OpenVLBI" On)
option(WITH_SHARED_SERVER "Add shared server libraries for OpenVLBI" On)
set (CMAKE_CXX_STANDARD 11)
set (CMAKE_C_STANDARD 11)
set (VLBI_VERSION_MAJOR 3)
set (VLBI_VERSION_MINOR 0)
set (VLBI_VERSION_RELEASE 4)
set (VLBI_VERSION_STRING "${VLBI_VERSION_MAJOR}.${VLBI_VERSION_MINOR}.${VLBI_VERSION_RELEASE}")
set (CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall --std=c++11 -O3 -fPIC")
set (CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Wall -O3 -fPIC")
set (LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
set (DATA_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}")
set (VLBI_CATALOG_PATH "${DATA_INSTALL_DIR}/OpenVLBI/cat/index.txt")

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/README.md" README )
string(REPLACE "[![CircleCi](https://circleci.com/gh/iliaplatone/OpenVLBI/tree/master.svg?style=shield)](https://circleci.com/gh/iliaplatone/OpenVLBI/?branch=master)" "" README "${README}")
string(REPLACE "[![Linux](https://github.com/iliaplatone/OpenVLBI/actions/workflows/default.yml/badge.svg)](https://github.com/iliaplatone/OpenVLBI/actions/workflows/default.yml)" "" README "${README}")
string(REGEX REPLACE "\n\# \([a-z:A-Z:0-9]*\)" "\n\n\\\\page page_\\1 \\1" README "${README}" )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/dsp/dsp.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/dsp.h )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/vlbi/vlbi.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/vlbi.h )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.cmake ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/openvlbi.iss.cmake ${CMAKE_CURRENT_BINARY_DIR}/openvlbi.iss )

include_directories( ${CFITSIO_INCLUDE_DIR})
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
    ${CMAKE_CURRENT_SOURCE_DIR}/vlbi/modelcollection.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vlbi/node.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vlbi/feature.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vlbi/baseline.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vlbi/stream.cpp
    )

set(vlbi_C_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/vlbi/file.c
    ${CMAKE_CURRENT_SOURCE_DIR}/vlbi/time.c
    ${CMAKE_CURRENT_SOURCE_DIR}/vlbi/base64.c
    ${CMAKE_CURRENT_SOURCE_DIR}/vlbi/matrix.c
    ${CMAKE_CURRENT_SOURCE_DIR}/vlbi/astro.c
    )

SET(dsp_C_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/dsp/align.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dsp/convert.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dsp/fft.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dsp/file.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dsp/filters.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dsp/stats.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dsp/buffer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dsp/convolution.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dsp/signals.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dsp/stream.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dsp/fits.c
    )

if(WITH_DOCS)
execute_process (COMMAND doxygen ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION ${DATA_INSTALL_DIR}/doc/libopenvlbi-dev)
endif(WITH_DOCS)
if(WITH_DSP)
add_library(opendsp SHARED ${dsp_C_SRC})
set_target_properties(opendsp PROPERTIES VERSION ${VLBI_VERSION_STRING} SOVERSION ${VLBI_VERSION_MAJOR})
target_link_libraries(opendsp ${FFTW3_LIBRARIES} ${M_LIB} ${CFITSIO_LIBRARIES} ${JPEG_LIBRARIES} ${PNG_LIBRARIES} ${ZLIB_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/dsp.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include/OpenVLBI)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dsp/fits_extensions.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include/OpenVLBI)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dsp/fits.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include/OpenVLBI)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dsp/sdfits.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include/OpenVLBI)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dsp/fitsidi.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include/OpenVLBI)
install(TARGETS opendsp LIBRARY DESTINATION ${LIB_INSTALL_DIR})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/FindDSP.cmake ${CMAKE_CURRENT_BINARY_DIR}/FindDSP.cmake )
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/FindDSP.cmake DESTINATION "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/cmake-${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}/Modules")
endif(WITH_DSP)

if(WITH_VLBI)
add_library(openvlbi SHARED ${vlbi_C_SRCS} ${vlbi_CXX_SRCS})
set_target_properties(openvlbi PROPERTIES VERSION ${VLBI_VERSION_STRING} SOVERSION ${VLBI_VERSION_MAJOR})
target_link_libraries(openvlbi opendsp ${M_LIB} ${CFITSIO_LIBRARIES} ${JPEG_LIBRARIES} ${PNG_LIBRARIES} ${ZLIB_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/vlbi.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include/OpenVLBI)
install(TARGETS openvlbi LIBRARY DESTINATION ${LIB_INSTALL_DIR})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/FindVLBI.cmake ${CMAKE_CURRENT_BINARY_DIR}/FindVLBI.cmake )
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/FindVLBI.cmake DESTINATION "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/cmake-${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}/Modules")

if(WITH_DATA)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/cat DESTINATION ${DATA_INSTALL_DIR}/OpenVLBI)
endif(WITH_DATA)

if(WITH_VLBI_SERVER)
add_library(openvlbi_server STATIC ${CMAKE_CURRENT_SOURCE_DIR}/vlbi_server.cpp ${CMAKE_CURRENT_SOURCE_DIR}/vlbi/instancecollection.cpp ${CMAKE_CURRENT_SOURCE_DIR}/vlbi/collection.cpp ${CMAKE_CURRENT_SOURCE_DIR}/vlbi/base64.c)
target_link_libraries(openvlbi_server ${CFITSIO_LIBRARIES} ${M_LIB} ${ZLIB_LIBRARY} openvlbi opendsp)
install(TARGETS openvlbi_server ARCHIVE DESTINATION ${LIB_INSTALL_DIR})

if(WITH_JSON_SERVER)
add_executable(vlbi_server_json ${CMAKE_CURRENT_SOURCE_DIR}/mjs.h ${CMAKE_CURRENT_SOURCE_DIR}/mjs.c ${CMAKE_CURRENT_SOURCE_DIR}/json.h ${CMAKE_CURRENT_SOURCE_DIR}/json.c ${CMAKE_CURRENT_SOURCE_DIR}/vlbi_server_json.cpp)
target_link_libraries(vlbi_server_json openvlbi ${CFITSIO_LIBRARIES} ${M_LIB} ${ZLIB_LIBRARY} ${CMAKE_DL_LIBS})
target_link_libraries(vlbi_server_json openvlbi_server)
install(TARGETS vlbi_server_json RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
endif(WITH_JSON_SERVER)

if(WITH_INDI_SERVER)
find_package(INDI 1.8.8)
if(INDI_LIBRARIES AND INDICLIENT_LIBRARIES)
find_package(Nova REQUIRED)
include_directories( ${INDI_INCLUDE_DIR})
add_executable(vlbi_server_indi ${CMAKE_CURRENT_SOURCE_DIR}/vlbi_server_indi.cpp)
target_link_libraries(vlbi_server_indi openvlbi indiclient stdc++fs ${CFITSIO_LIBRARIES} ${M_LIB} ${ZLIB_LIBRARY})
target_link_libraries(vlbi_server_indi openvlbi_server)
install(TARGETS vlbi_server_indi RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
endif(INDI_LIBRARIES AND INDICLIENT_LIBRARIES)
endif(WITH_INDI_SERVER)

if(WITH_DUMMY_SERVER)
add_executable(vlbi_server_dummy ${CMAKE_CURRENT_SOURCE_DIR}/vlbi_server_dummy.cpp)
target_link_libraries(vlbi_server_dummy openvlbi ${CFITSIO_LIBRARIES} ${M_LIB} ${ZLIB_LIBRARY})
target_link_libraries(vlbi_server_dummy openvlbi_server)
install(TARGETS vlbi_server_dummy RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
endif(WITH_DUMMY_SERVER)

endif(WITH_VLBI_SERVER)

endif(WITH_VLBI)
