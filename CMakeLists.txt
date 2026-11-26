cmake_minimum_required(VERSION 3.30)
project(LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)#-std=c++26, not -std=gnu++26
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(
	-Wall
	-freflection
	-fno-exceptions
	-fno-rtti
	-fconstexpr-ops-limit=4294967295
	-fconstexpr-loop-limit=2147483647
	-ftemplate-depth=65535
	-march=native
	-fmax-errors=1
)
add_link_options(
	-mwindows
	-municode
)
add_executable(main main.cpp)