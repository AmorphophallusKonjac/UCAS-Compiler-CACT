cmake_minimum_required(VERSION 3.5)

project(Compiler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)

include_directories(deps/antlr4-runtime/)
include_directories(grammar)
include_directories(src)


FILE(GLOB_RECURSE SOURCE_FILES "src/*.cpp" "grammar/*.cpp")

FILE(GLOB_RECURSE ANTLR_LIB_FILES "deps/antlr4-runtime/*.cpp")

add_custom_target(
    gen_files_form_g4
    COMMAND echo "Using antlr4 to generate files from grammar file..."
    COMMAND java -jar ../deps/antlr-4.13.1-complete.jar -Dlanguage=Cpp ../grammar/CACT.g4 -visitor -no-listener
    VERBATIM
)

add_definitions(-w -g)
add_library(antlr STATIC ${ANTLR_LIB_FILES})
add_executable(compiler ${SOURCE_FILES})
add_dependencies(compiler gen_files_form_g4)
find_package(Threads REQUIRED)
target_link_libraries(compiler antlr Threads::Threads)

enable_testing()

add_test(
    NAME syntax_test
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/testbench/syntax_test.sh
)

set_tests_properties(syntax_test PROPERTIES TIMEOUT 10)