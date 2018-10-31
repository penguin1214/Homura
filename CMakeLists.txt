cmake_minimum_required(VERSION 3.2)
project(Homura)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_VERBOSE_MAKEFILE 1)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

include_directories("include/" "include/thirdparty/")

# core library
file(GLOB_RECURSE CORE_SRC "src/core/*.cpp")
add_library(core STATIC ${CORE_SRC})

file(GLOB_RECURSE RENDERER_SRC "src/renderer/*.cpp")
add_library(renderer ${RENDERER_SRC})
target_link_libraries(renderer core)

file(GLOB_RECURSE GUI_SRC "src/GUI/*.cpp")
add_executable(GUI ${GUI_SRC})

# opengl
find_package(OpenGL REQUIRED)
if (OPENGL_FOUND)
	message(STATUS "OpenGL found.")
	include_directories(${OPENGL_INCLUDE_DIRS})
endif()

# glew
if (MSVC)
	set(CMAKE_PREFIX_PATH "dependencies/glew")
endif()

find_package(GLEW REQUIRED)
if (GLEW_FOUND)
	message(STATUS "GLEW found.")
	include_directories(${GLEW_INCLUDE_DIRS})
endif()

# glut
set(CMAKE_PREFIX_PATH "dependencies/freeglut")
find_package(GLUT)
if (GLUT_FOUND)
	message(STATUS "GLUT found.")
	include_directories(${GLUT_INCLUDE_DIRS})
endif()

# glfw
if (MSVC)
	set(CMAKE_PREFIX_PATH "dependencies/glfw")
	find_path(GLFW_INCLUDE_DIRS GLFW/glfw3.h)
	if (GLFW_INCLUDE_DIRS)
		include_directories(${GLFW_INCLUDE_DIRS})
	endif()
	find_library(GLFW_LIBRARIES glfw3)
	if (GLFW_LIBRARIES)
		message(STATUS "glfw3 found.")
	endif()
else()
	find_package(glfw3 REQUIRED)
	if (glfw3_FOUND)
		message(STATUS "glfw3 found.")
		include_directories(${GLFW_INCLUDE_DIRS})
	endif()
 endif()

if (APPLE)
	target_link_libraries(GUI glfw)
#	target_link_libraries(GUI "-framework Cocoa")
#	target_link_libraries(GUI "-framework OpenGL")
endif()

if (NOT MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()

target_link_libraries(
	GUI
	${OPENGL_LIBRARIES}
	${GLEW_LIBRARIES}
	${GLFW_LIBRARIES}
	${GLUT_LIBRARIES}
	core
	renderer
)

set(HOMURA_SRC src/Homura.cpp)
add_executable(${PROJECT_NAME} ${HOMURA_SRC})

target_link_libraries(
	${PROJECT_NAME}
	core
	renderer
)