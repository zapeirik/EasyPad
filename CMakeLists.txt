cmake_minimum_required(VERSION 3.16)
project(EasyPad VERSION 1.0.0 LANGUAGES CXX)

set(EASYPAD_DEFAULT_WIDTH  600)
set(EASYPAD_DEFAULT_HEIGHT 200)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/Config.h
    @ONLY
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(APP_ICON ${CMAKE_CURRENT_SOURCE_DIR}/EasyPad.icns)
set_source_files_properties(${APP_ICON} PROPERTIES
    MACOSX_PACKAGE_LOCATION "Resources")

qt_add_executable(${PROJECT_NAME} MACOSX_BUNDLE main.cpp ${APP_ICON})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

set_target_properties(EasyPad PROPERTIES
    MACOSX_BUNDLE_ICON_FILE EasyPad.icns
    MACOSX_BUNDLE_BUNDLE_NAME EasyPad)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets)
