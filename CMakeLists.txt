cmake_minimum_required(VERSION 3.5)

project(BugReproduction01 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        MainWindow.cpp
        MainWindow.h
        MainWindow.ui
        Common.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(BugReproduction01
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET BugReproduction01 APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(BugReproduction01 SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(BugReproduction01
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(BugReproduction01 PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.BugReproduction01)
endif()
set_target_properties(BugReproduction01 PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)


include(GNUInstallDirs)
install(TARGETS BugReproduction01
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(BugReproduction01)
endif()

################################################################################

add_definitions(
    -DQT_NO_CAST_FROM_ASCII
    -DQT_USE_QSTRINGBUILDER
    -DQT_USE_FAST_OPERATOR_PLUS)
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug" OR "${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
    add_definitions(-DDEBUG -D_DEBUG)
endif()

if(MSVC)
    target_compile_options(BugReproduction01 PRIVATE
          $<$<COMPILE_LANGUAGE:CXX>:/FI "${CMAKE_CURRENT_SOURCE_DIR}/Common.h" >
    )
else()
    target_compile_options(BugReproduction01 PRIVATE
          $<$<COMPILE_LANGUAGE:CXX>:-include "${CMAKE_CURRENT_SOURCE_DIR}/Common.h" >
    )
endif()

if(MSVC)
    file(GLOB_RECURSE ALL_SOURCES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.cpp" "*.h")
    foreach(source_file ${ALL_SOURCES})
        cmake_path(GET source_file PARENT_PATH source_path)
        string(REPLACE "/" "\\" group_path "${source_path}")  # Escape backslashes for VS
        if("${source_file}" MATCHES "\\.cpp$")
            source_group("Source Files\\${group_path}" FILES "${source_file}")
        elseif("${source_file}" MATCHES "\\.h$")
            source_group("Header Files\\${group_path}" FILES "${source_file}")
        endif()
    endforeach()

    file(GLOB_RECURSE ALL_UI_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.ui")
    source_group("Qt\\UI" FILES ${ALL_UI_FILES})
    foreach(source_file ${ALL_UI_FILES})
        cmake_path(REPLACE_EXTENSION source_file ".h")
        cmake_path(GET source_file FILENAME source_filename)
        set(source_filename "ui_${source_filename}")
        cmake_path(GET source_file PARENT_PATH source_path)
        foreach(build_type Release RelWithDebInfo MinSizeRel Debug)
            source_group("Qt\\MOC\\${build_type}" FILES "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}_autogen/include_${build_type}/${source_path}/${source_filename}")
        endforeach()
    endforeach()

    foreach(build_type Release RelWithDebInfo MinSizeRel Debug)
        source_group("Qt\\MOC\\${build_type}" FILES "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}_autogen/mocs_compilation_${build_type}.cpp")
        source_group("CMake Rules\\Stamps" FILES "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}_autogen/autouic_${build_type}.stamp")
    endforeach()
endif()
