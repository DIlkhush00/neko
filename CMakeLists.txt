cmake_minimum_required(VERSION 3.28)

#####
## Project
#####
project(neko VERSION 1.0.0 LANGUAGES CXX)


######
## Set CXX Standard
######
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#######
## Add the executable
#######
add_executable(
    ${CMAKE_PROJECT_NAME}
    src/main.cpp
    src/lexer/lexer.cpp
)