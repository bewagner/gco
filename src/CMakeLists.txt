add_executable(${PROJECT_NAME} main.cpp)

set(BUNDLE_LIBGIT2 ON CACHE BOOL "" FORCE)
add_subdirectory(libgit2cpp)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libgit2cpp/include)
target_link_libraries(${PROJECT_NAME} git2cpp)

find_package(Boost 1.46.0 REQUIRED COMPONENTS filesystem)
target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${Boost_FILESYSTEM_LIBRARIES})




