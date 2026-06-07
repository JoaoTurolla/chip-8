cmake_minimum_required(VERSION 4.2.3)
project(Chip8)

set(CMAKE_CXX_STANDARD 11)

set(SDL_SHARED OFF CACHE BOOL "Force SDL to build static" FORCE)
set(SDL_STATIC ON CACHE BOOL "Force SDL to build static" FORCE)
set(SDL_PIPEWIRE OFF CACHE BOOL "Disable PipeWire" FORCE)
add_subdirectory(thirdParty/SDL2-2.0.20 EXCLUDE_FROM_ALL)
add_subdirectory(thirdParty/glad EXCLUDE_FROM_ALL)
add_subdirectory(thirdParty/imgui-1.88 EXCLUDE_FROM_ALL)

add_executable(
	Chip8
	source/Chip8.cpp
	source/main.cpp
	source/Platform.cpp)

target_compile_options(Chip8 PRIVATE -Wall)

target_link_libraries(Chip8 PRIVATE glad SDL2-static imgui)
