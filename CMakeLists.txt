cmake_minimum_required(VERSION 2.6)

PROJECT(OpenVLBI C CXX)
LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/")

include(GNUInstallDirs)
include(ExternalProject)

find_package(CFITSIO REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Threads REQUIRED)
option(WITH_INDI_CLIENT "Add INDI client for OpenVLBI" ON)

set (VLBI_VERSION_MAJOR 1)
set (VLBI_VERSION_MINOR 4)
set (VLBI_VERSION_STRING "${VLBI_VERSION_MAJOR}.${VLBI_VERSION_MINOR}.0")

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/")
LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake_modules/")

add_definitions(-Wall -O3 -g)

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/README.md" README )
string(REPLACE "\n#* " "\n\n\\page " README "${README}" )
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
    ${CMAKE_CURRENT_SOURCE_DIR}/vlbi/sync.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vlbi/matrix.cpp
    )

set(vlbi_C_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/vlbi/align.c
    ${CMAKE_CURRENT_SOURCE_DIR}/vlbi/astro.c
    ${CMAKE_CURRENT_SOURCE_DIR}/vlbi/time.c
    ${CMAKE_CURRENT_SOURCE_DIR}/vlbi/base64.c
    )

SET(dsp_C_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/dsp/buffer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dsp/convert.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dsp/fft.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dsp/filters.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dsp/signals.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dsp/convolution.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dsp/stats.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dsp/stream.c
    )

execute_process (COMMAND doxygen ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_library(vlbi SHARED ${dsp_C_SRC} ${vlbi_C_SRCS} ${vlbi_CXX_SRCS})
set_target_properties(vlbi PROPERTIES VERSION ${VLBI_VERSION_STRING} SOVERSION ${VLBI_VERSION_MAJOR})
target_link_libraries(vlbi ${M_LIB} ${ZLIB_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/vlbi.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include/OpenVLBI)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/dsp.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include/OpenVLBI)
install(TARGETS vlbi LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/scripts/vlbi_server DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

if(WITH_INDI_CLIENT)
find_package(INDI REQUIRED)
find_package(INDICLIENT REQUIRED)
if(INDI_LIBRARIES AND INDICLIENT_LIBRARIES)
include_directories( ${INDI_INCLUDE_DIR})
add_definitions(-DINDI_CLIENT)
add_executable(vlbi_client_indi ${CMAKE_CURRENT_SOURCE_DIR}/vlbi_client.cpp ${CMAKE_CURRENT_SOURCE_DIR}/vlbi_client_indi.cpp)
target_link_libraries(vlbi_client_indi vlbi ${CMAKE_THREAD_LIBS_INIT} ${INDICLIENT_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${CFITSIO_LIBRARIES} ${M_LIB} ${ZLIB_LIBRARY})
install(TARGETS vlbi_client_indi RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
endif(INDI_LIBRARIES AND INDICLIENT_LIBRARIES)
endif(WITH_INDI_CLIENT)
