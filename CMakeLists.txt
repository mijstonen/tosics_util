project(util)

add_executable(demos util_demos.cpp)

install(TARGETS demos RUNTIME DESTINATION bin)
