find_package(V4L2 REQUIRED)
find_package(OpenCV REQUIRED)
find_package(Boost COMPONENTS system filesystem REQUIRED)

file(GLOB SOURCES *.h *.cpp io/video/*.h io/video/*.cpp io/video/linux/*.h io/video/linux/*.cpp)


INCLUDE_DIRECTORIES(include ${OpenCV_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} ${V4L2_INCLUDE_DIRS})

add_library(gago SHARED ${SOURCES})
set_target_properties(gago PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(gago ${V4L2_LIBRARIES} ${Boost_FILESYSTEM_LIBRARY}
        ${Boost_SYSTEM_LIBRARY} ${OpenCV_LIBRARIES} pthread)


file(GLOB PUBLIC_HEADERS *.h io/video/*.h io/video/linux/*.h)

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/gago

        FILES_MATCHING PATTERN *.h)

install(TARGETS gago
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_PREFIX}/include/gago
        CONFIGURATIONS Release)