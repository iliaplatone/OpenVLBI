cmake_minimum_required(VERSION 2.8...3.13)

PROJECT(OpenVLBI C CXX)
LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/")

include(GNUInstallDirs)
include(ExternalProject)

find_package(CFITSIO REQUIRED)
find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)
find_package(FFTW3 REQUIRED)
find_package(Threads REQUIRED)
option(WITH_VLBI_SERVER "Add OpenVLBI servers" ON)
option(WITH_INDI_SERVER "Add INDI server for OpenVLBI" Off)
option(WITH_DUMMY_SERVER "Add dummy server for OpenVLBI" ON)
option(WITH_JSON_SERVER "Add JSON server for OpenVLBI" ON)

set (VLBI_VERSION_MAJOR 1)
set (VLBI_VERSION_MINOR 22)
set (VLBI_VERSION_RELEASE 9)
set (VLBI_VERSION_STRING "${VLBI_VERSION_MAJOR}.${VLBI_VERSION_MINOR}.${VLBI_VERSION_RELEASE}")
set(LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
set(DATA_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}")
set(VLBI_CATALOG_PATH "${DATA_INSTALL_DIR}/OpenVLBI/cat/index.txt")

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/")
LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake_modules/")

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/README.md" README )
string(REPLACE "[![CircleCi](https://circleci.com/gh/iliaplatone/OpenVLBI/tree/master.svg?style=shield)](https://circleci.com/gh/iliaplatone/OpenVLBI/?branch=master)" "" README "${README}")
string(REPLACE "[![Linux](https://github.com/iliaplatone/OpenVLBI/actions/workflows/default.yml/badge.svg)](https://github.com/iliaplatone/OpenVLBI/actions/workflows/default.yml)" "" README "${README}")
string(REGEX REPLACE "\n\# \([a-z:A-Z:0-9]*\)" "\n\n\\\\page page_\\1 \\1" README "${README}" )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/dsp/dsp.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/dsp.h )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/vlbi/vlbi.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/vlbi.h )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.cmake ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile )

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

execute_process (COMMAND doxygen ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
add_library(opendsp SHARED ${dsp_C_SRC} ${vlbi_CXX_SRCS})
set_target_properties(opendsp PROPERTIES VERSION ${VLBI_VERSION_STRING} SOVERSION ${VLBI_VERSION_MAJOR})
target_link_libraries(opendsp ${FFTW3_LIBRARIES} ${M_LIB} ${CFITSIO_LIBRARIES} ${JPEG_LIBRARIES} ${PNG_LIBRARIES} ${ZLIB_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
add_library(openvlbi SHARED ${vlbi_C_SRCS} ${vlbi_CXX_SRCS})
set_target_properties(openvlbi PROPERTIES VERSION ${VLBI_VERSION_STRING} SOVERSION ${VLBI_VERSION_MAJOR})
target_link_libraries(openvlbi opendsp ${M_LIB} ${CFITSIO_LIBRARIES} ${JPEG_LIBRARIES} ${PNG_LIBRARIES} ${ZLIB_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/vlbi.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include/OpenVLBI)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/dsp.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include/OpenVLBI)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/vlbi_server.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include/OpenVLBI)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dsp/fits_extensions.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include/OpenVLBI)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dsp/fits.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include/OpenVLBI)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dsp/sdfits.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include/OpenVLBI)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dsp/fitsidi.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include/OpenVLBI)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/cat DESTINATION ${DATA_INSTALL_DIR}/OpenVLBI)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION ${DATA_INSTALL_DIR}/doc/libopenvlbi-dev)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/FindVLBI.cmake DESTINATION ${DATA_INSTALL_DIR}/cmake-${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}/Modules)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/FindDSP.cmake DESTINATION ${DATA_INSTALL_DIR}/cmake-${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}/Modules)
install(TARGETS openvlbi LIBRARY DESTINATION ${LIB_INSTALL_DIR})
install(TARGETS opendsp LIBRARY DESTINATION ${LIB_INSTALL_DIR})

if(WITH_VLBI_SERVER)
add_library(openvlbi_server STATIC ${CMAKE_CURRENT_SOURCE_DIR}/vlbi_server.cpp)
install(TARGETS openvlbi_server ARCHIVE DESTINATION ${LIB_INSTALL_DIR})

if(WITH_JSON_SERVER)
add_executable(vlbi_server_json ${CMAKE_CURRENT_SOURCE_DIR}/json.h ${CMAKE_CURRENT_SOURCE_DIR}/json.c ${CMAKE_CURRENT_SOURCE_DIR}/vlbi_server_json.cpp ${CMAKE_CURRENT_SOURCE_DIR}/vlbi/instancecollection.cpp)
target_link_libraries(vlbi_server_json openvlbi openvlbi_server ${CFITSIO_LIBRARIES} ${M_LIB} ${ZLIB_LIBRARY})
install(TARGETS vlbi_server_json RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
endif(WITH_JSON_SERVER)

if(WITH_INDI_SERVER)
find_package(INDI REQUIRED)
find_package(INDICLIENT REQUIRED)
find_package(Nova REQUIRED)
if(INDI_LIBRARIES AND INDICLIENT_LIBRARIES)
include_directories( ${INDI_INCLUDE_DIR})
add_executable(vlbi_server_indi ${CMAKE_CURRENT_SOURCE_DIR}/vlbi_server_indi.cpp ${CMAKE_CURRENT_SOURCE_DIR}/vlbi/instancecollection.cpp)
target_link_libraries(vlbi_server_indi openvlbi openvlbi_server ${INDICLIENT_LIBRARIES} ${CFITSIO_LIBRARIES} ${M_LIB} ${ZLIB_LIBRARY})
install(TARGETS vlbi_server_indi RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
endif(INDI_LIBRARIES AND INDICLIENT_LIBRARIES)
endif(WITH_INDI_SERVER)

if(WITH_DUMMY_SERVER)
add_executable(vlbi_server_dummy ${CMAKE_CURRENT_SOURCE_DIR}/vlbi_server_dummy.cpp ${CMAKE_CURRENT_SOURCE_DIR}/vlbi/instancecollection.cpp)
target_link_libraries(vlbi_server_dummy openvlbi openvlbi_server ${M_LIB} ${ZLIB_LIBRARY})
install(TARGETS vlbi_server_dummy RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
endif(WITH_DUMMY_SERVER)
endif(WITH_VLBI_SERVER)
