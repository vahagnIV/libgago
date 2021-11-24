#set(CUDA_TOOLKIT_ROOT_DIR /usr/local/cuda-10.1)
find_package(Boost COMPONENTS system filesystem REQUIRED)




file(GLOB SOURCES *.h *.cpp io/video/*.h io/video/*.cpp)
list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_LIST_DIR}/test.cpp)
if (UNIX AND NOT APPLE)
    find_package(V4L2 REQUIRED)
    find_package(JPEG REQUIRED)
    INCLUDE_DIRECTORIES(${JPEG_INCLUDE_DIR})
    file(GLOB OS_SPECIFIC io/video/linux/*.h io/video/linux/*.cpp io/video/linux/codecs/* )
else ()
    find_package(OpenCV REQUIRED)	
    INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
    message("NOT LINUX")
    file(GLOB OS_SPECIFIC io/video/cross_platform/*.h io/video/cross_platform/*.cpp)
endif ()


INCLUDE_DIRECTORIES(include ${CMAKE_CURRENT_SOURCE_DIR} ${V4L2_INCLUDE_DIRS})

add_library(gago SHARED ${SOURCES} ${OS_SPECIFIC})
set_target_properties(gago PROPERTIES LINKER_LANGUAGE CXX)

find_package(OpenCV REQUIRED)
INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
add_executable(test test.cpp)
target_link_libraries(test gago ${OpenCV_LIBRARIES})

target_link_libraries(gago ${V4L2_LIBRARIES} ${Boost_FILESYSTEM_LIBRARY}
        ${Boost_SYSTEM_LIBRARY} pthread)
if (UNIX AND NOT APPLE)
    target_link_libraries(gago ${JPEG_LIBRARY})
endif ()

file(GLOB PUBLIC_HEADERS *.h io/video/*.h io/video/linux/*.h)


generate_export_header(gago)
set_property(TARGET gago PROPERTY VERSION ${LibGago_VERSION})
set_property(TARGET gago PROPERTY SOVERSION 1)
set_property(TARGET gago PROPERTY INTERFACE_gao_MAJOR_VERSION 1)
set_property(TARGET gago APPEND PROPERTY
        COMPATIBLE_INTERFACE_STRING gago_MAJOR_VERSION)


install(TARGETS gago
        EXPORT gagoTargets
        LIBRARY
        DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/
        PUBLIC_HEADER
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/gago
        CONFIGURATIONS Release)

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/gago
        FILES_MATCHING PATTERN *.h)


include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/LibGago/LibGagoConfigVersion.cmake"
        VERSION ${LibGago_VERSION}
        COMPATIBILITY AnyNewerVersion)

export(EXPORT gagoTargets
        FILE "${CMAKE_CURRENT_BINARY_DIR}/LibGago/GagoTargets.cmake")

configure_file(${CMAKE_MODULE_PATH}/LibGagoConfig.cmake "${CMAKE_CURRENT_BINARY_DIR}/LibGago/LibGagoConfig.cmake" COPYONLY)

set(ConfigPackageLocation lib/cmake/LibGago)

install(EXPORT gagoTargets
        FILE
        gagoTargets.cmake
        DESTINATION
        ${ConfigPackageLocation})

install(FILES
        ${CMAKE_MODULE_PATH}/LibGagoConfig.cmake
        "${CMAKE_CURRENT_BINARY_DIR}/LibGago/LibGagoConfigVersion.cmake"
        DESTINATION
        ${ConfigPackageLocation}
        COMPONENT
        Devel)
