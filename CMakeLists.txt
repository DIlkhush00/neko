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


######
## Clang-Format Target
######
find_program(CLANG_FORMAT_EXE clang-format)

if (CLANG_FORMAT_EXE)
    add_custom_target(format
        COMMAND sh ${CMAKE_SOURCE_DIR}/format.sh
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running clang-format"
    )
else()
    message(STATUS "clang-format not found; format target disabled")
endif()


#######
## Add the executable
#######
add_executable(
    ${CMAKE_PROJECT_NAME}
    src/main.cpp
    src/lexer/lexer.cpp
    src/parser/parser.cpp
    src/sema/semantic_analyzer.cpp
    src/sema/symbol_table.cpp
)