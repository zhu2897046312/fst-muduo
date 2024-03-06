cmake_minimum_required(VERSION 3.0)
project(fst_muduo)

# 设置项目可执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#设置编译成的so库路径
set(LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

#设置调试信息
set(CMAKE_BUILD_TYPE "DEBUG")

# 设置C编译器
set(CMAKE_C_COMPILER "clang")

# 设置C++编译器
set(CMAKE_CXX_COMPILER "clang++")

include_directories(${PROJECT_SOURCE_DIR}/src/include)

#定义参与编译的源代码文件
aux_source_directory(${PROJECT_SOURCE_DIR}/src SRC_LIST)

#编译动态库
add_library(fst_muduo SHARED ${SRC_LIST})

add_subdirectory(src)
add_subdirectory(test)

