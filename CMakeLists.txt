cmake_minimum_required(VERSION 3.10)

project(LibGago C CXX)
set(CMAKE_CXX_STANDARD 11)

set(LibGago_VERSION 1.0.0)

include(GenerateExportHeader)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

add_subdirectory(src)
