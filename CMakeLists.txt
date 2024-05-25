cmake_minimum_required(VERSION 3.5)
#############
project(util)
#############


#____________________________________________________________________________________
# define

set(PRJ_PARENT "/home/michel/Projects/Kdevelop")
 set(INCLUDE_PATH "${PRJ_PARENT}/util/")
set(CMAKE_CXX_COMPILER /usr/local/bin/g++)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ")
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG -O0 -ggdb")

# to link with libtrace ?  link_directories(/usr/local/lib)  or use find_package??? and find_library???

add_definitions(
)

add_compile_options(
    -march=native
    -save-temps
    -std=gnu++23
    -fconcepts
    -ggdb
    -I${INCLUDE_PATH}
    -include stdinc.hpp
    -Wall -Wextra -Wold-style-cast
    -x c++
    )
#____________________________________________________________________________________
# compile

add_library(
    util STATIC
    util.cpp
)

add_executable(
    demos
    util_demos.cpp
)

#____________________________________________________________________________________
# link

target_link_libraries(demos
  util
  stdc++fs
)

#____________________________________________________________________________________
# test

enable_testing()

add_test(demos
  demos
)

#____________________________________________________________________________________
# install

# install(TARGETS demos RUNTIME DESTINATION bin)

