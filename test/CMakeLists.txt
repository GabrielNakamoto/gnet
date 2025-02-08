cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CXX_EXTENSIONS ON)

project(gnet LANGUAGES CXX)

add_executable(gnet
	test.cpp
	../src/socket.cpp
	../src/node.cpp
	)

target_include_directories(gnet PUBLIC ../src)
