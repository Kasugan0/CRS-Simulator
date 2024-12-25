# 最低CMake版本要求
cmake_minimum_required(VERSION 3.31)

# 项目名称和版本
project(ATM VERSION 0.5 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 添加编译选项
add_compile_options(-Wall -Wextra -pedantic -ggdb)

# 启用地址消毒器 (fsanitize=undefined)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")

# 添加源文件
set(SOURCE_FILES
    Account.cpp
    main.cpp
    Ui.cpp
)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
