cmake_minimum_required(VERSION 3.12)

project(audio-network-analyzer VERSION 0.1.0)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

if (MSVC)
	add_compile_options(/W4 /WX)
else()
	add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

find_package(PulseAudio REQUIRED)

add_subdirectory(src)
