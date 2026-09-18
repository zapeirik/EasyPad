cmake_minimum_required(VERSION 3.16)
project(EasyPad LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(APP_ICON ${CMAKE_CURRENT_SOURCE_DIR}/EasyPad.icns)
set_source_files_properties(${APP_ICON} PROPERTIES
    MACOSX_PACKAGE_LOCATION "Resources")

# qt_add_executable(easypad main.cpp)
qt_add_executable(EasyPad MACOSX_BUNDLE main.cpp ${APP_ICON})

set_target_properties(EasyPad PROPERTIES
    MACOSX_BUNDLE_ICON_FILE EasyPad.icns
    MACOSX_BUNDLE_BUNDLE_NAME EasyPad)

target_link_libraries(EasyPad PRIVATE Qt6::Widgets)
