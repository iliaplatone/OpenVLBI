cmake_minimum_required(VERSION 3.16)

project(openvlbi-gui VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/graph/arrow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graph/arrow.h
    ${CMAKE_CURRENT_SOURCE_DIR}/graph/diagramitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graph/diagramitem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/graph/diagramscene.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graph/diagramscene.h
    ${CMAKE_CURRENT_SOURCE_DIR}/graph/diagramscene.pro
    ${CMAKE_CURRENT_SOURCE_DIR}/graph/diagramscene.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/graph/diagramtextitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graph/diagramtextitem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/graph/flowchart.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graph/flowchart.h
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mainwindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mainwindow.h
    ${CMAKE_CURRENT_SOURCE_DIR}/mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(openvlbi-gui
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET openvlbi-gui APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(openvlbi-gui SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(openvlbi-gui
            ${PROJECT_SOURCES}
        )
        if(WIN32)
            target_link_libraries(openvlbi-gui PRIVATE comctl32)
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,--subsystem,windows")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--subsystem,windows")
            set(CMAKE_LD_FLAGS "${CMAKE_LD_FLAGS} -Wl,--subsystem,windows -static-libstdc++ -static-libgcc")
            set_target_properties(openvlbi-gui PROPERTIES WIN32_EXECUTABLE TRUE)
        endif()
    endif()
endif()

target_link_libraries(openvlbi-gui PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.openvlbi-gui)
endif()
set_target_properties(openvlbi-gui PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS openvlbi-gui
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(openvlbi-gui)
endif()
