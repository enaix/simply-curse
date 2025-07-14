cmake_minimum_required(VERSION 3.27)
project(curse)

set(CMAKE_CXX_STANDARD 20)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -Werror=return-type")
endif ()

include_directories("./lib")

add_library(curse INTERFACE lib/curse.h)

target_include_directories(curse INTERFACE lib)
set_property(TARGET curse PROPERTY LINKER_LANGUAGE CXX)

# Tests
add_executable(ui_test tests/ui.cpp lib/curse.h)
target_link_libraries(ui_test INTERFACE curse)