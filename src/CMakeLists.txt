# Generic test that uses conan libs

add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/args)


#set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
#add_subdirectory(libgit2)
#target_link_libraries(${PROJECT_NAME} git2)
#target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libgit2/include)

set(BUNDLE_LIBGIT2 ON CACHE BOOL "" FORCE)
add_subdirectory(libgit2cpp)
target_link_libraries(${PROJECT_NAME} git2cpp)
#target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libgit2cpp/include)

target_link_libraries(${PROJECT_NAME} stdc++fs)





