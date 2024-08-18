cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_FLAGS "-std=c++20")

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/vendor/imgui)
include_directories(${PROJECT_SOURCE_DIR}/vendor/imgui/backends)

find_library(DDCUTIL NAMES ddcutil HINTS /usr/lib/x86_64_linux-gnu)

if(NOT DDCUTIL)
    message("\nddcutil not found\n")
endif()

#include(FetchContent)
#FetchContent_Declare(imgui_external
#	URL https://github.com/ocornut/imgui/archive/refs/tags/v1.91.0.tar.gz
#	EXCLUDE_FROM_ALL
#)
#FetchContent_MakeAvailable(imgui_external)"

add_library(imgui
	${PROJECT_SOURCE_DIR}/vendor/imgui/imgui.cpp
	${PROJECT_SOURCE_DIR}/vendor/imgui/imgui_draw.cpp
	${PROJECT_SOURCE_DIR}/vendor/imgui/imgui_tables.cpp
	${PROJECT_SOURCE_DIR}/vendor/imgui/imgui_widgets.cpp
	${PROJECT_SOURCE_DIR}/vendor/imgui/imgui_demo.cpp
	${PROJECT_SOURCE_DIR}/vendor/imgui/backends/imgui_impl_sdl2.cpp
	${PROJECT_SOURCE_DIR}/vendor/imgui/backends/imgui_impl_opengl3.cpp
)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} main.cpp dispmanager.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${DDCUTIL} PUBLIC imgui ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES})