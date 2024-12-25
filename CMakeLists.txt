cmake_minimum_required(VERSION 3.31)

project(CRS-Simulator VERSION 0.5 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -pedantic -ggdb)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")

set(SOURCE_FILES
    Account.cpp
    main.cpp
    Ui.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
