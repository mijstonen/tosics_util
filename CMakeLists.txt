#____________________________________________________________________________________
# define

cmake_minimum_required(VERSION 2.8)

project(util)

link_directories(/usr/local/lib)

# Compile (progressive) C++11 syntax and do (almost) every thinkable possible warning
# stdinc.h is precompiled, enforce loading of it right from the start
add_definitions(-std=gnu++1y -xc++ -include ../util/stdinc.hpp -Wall -Wextra -Wold-style-cast)
## disabled  -Weffc++  -Wno-variadic-macros -Wpedantic causes too much noise,
#   consider it as a regulary but low prio task and to be enabled in the review phase

#____________________________________________________________________________________
# compile

add_library(utils STATIC
    utils.cpp
)


add_executable(demos
    util_demos.cpp
)

#____________________________________________________________________________________
# link

target_link_libraries(demos
  utils
)


#____________________________________________________________________________________
# test

enable_testing()

add_test(demos
  demos
)

#____________________________________________________________________________________
# install

install(TARGETS demos RUNTIME DESTINATION bin)
