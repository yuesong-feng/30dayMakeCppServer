file(GLOB_RECURSE pine_src ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_library(pine_shared SHARED ${pine_src})
