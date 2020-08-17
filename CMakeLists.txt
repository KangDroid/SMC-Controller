cmake_minimum_required(VERSION 3.14)
project(SMCController)
set( CMAKE_EXE_LINKER_FLAGS "-framework IOKit" )
add_executable(SMCController main.cpp smc.c)