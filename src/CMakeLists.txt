add_executable(${PROJECT_NAME} main.cpp fts_fuzzy_match.h)

include(FetchContent)
FetchContent_Declare(
        git2cpp
        GIT_REPOSITORY "https://github.com/AndreyG/libgit2cpp"
        GIT_TAG "45762e692d202cb80b3d07f5da7e2f85191b6608"
        CMAKE_ARGS -DBUNDLE_LIBGIT2:BOOL=ON
)
FetchContent_Declare(
        doctest
        GIT_REPOSITORY "https://github.com/onqtam/doctest"
        GIT_TAG "932a2ca50666138256dae56fbb16db3b1cae133a"
)
FetchContent_MakeAvailable(git2cpp doctest)
target_link_libraries(${PROJECT_NAME} git2cpp doctest)



find_package(Boost 1.46.0 REQUIRED COMPONENTS filesystem)
target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${Boost_FILESYSTEM_LIBRARIES})




