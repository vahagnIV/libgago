find_package(V4L2 REQUIRED)
find_package(OpenCV REQUIRED)
find_package(Boost COMPONENTS system filesystem REQUIRED)
file(GLOB SOURCES *.h *.cpp io/video/*.h io/video/*.cpp io/video/linux/*.h io/video/linux/*.cpp algorithm/*.hpp algorithm/*.cpp)

list(REMOVE_ITEM ${SOURCES} test.cpp)

INCLUDE_DIRECTORIES(include ${OpenCV_INCLUDE_DIRS} ../external ${CMAKE_CURRENT_SOURCE_DIR})

add_library(gago SHARED ${SOURCES})

set_target_properties(gago PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(gago ${V4L2_LIBRARIES} ${Boost_FILESYSTEM_LIBRARY}
        ${Boost_SYSTEM_LIBRARY} ${OpenCV_LIBRARIES} pthread)

add_executable(test ${SOURCES} test.cpp)
set_target_properties(test PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(test ${V4L2_LIBRARIES} ${Boost_FILESYSTEM_LIBRARY}
        ${Boost_SYSTEM_LIBRARY} ${OpenCV_LIBRARIES} pthread)
