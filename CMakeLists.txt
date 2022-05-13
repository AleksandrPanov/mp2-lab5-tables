cmake_minimum_required(VERSION 3.0.0)
project(table LANGUAGES CXX)

file(GLOB SRC
	"table/*.cpp"
	"table/*.hpp" 
	"table/*.h"
)

add_executable(table ${SRC})
