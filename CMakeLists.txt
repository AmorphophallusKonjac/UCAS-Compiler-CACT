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

#自定义一个生成目标
add_custom_target(
        gen_files_form_g4
        COMMAND echo "Using antlr4 to generate files from grammar file..."
        COMMAND java -jar ../deps/antlr-4.13.1-complete.jar -Dlanguage=Cpp ../grammar/CACT.g4 -visitor -no-listener
        VERBATIM
)

#添加编译参数
add_definitions(-w -g)

#根据source_files编译compiler(executable添加一个可执行目标)
#同时生成完可链接的antlr静态库(antlr4)之后完成
add_executable(compiler ${SOURCE_FILES})
add_dependencies(compiler antlr)

#寻找外部库Threads，并且创建静态库目标antlr，链接到compiler
find_package(Threads REQUIRED)
add_library(antlr STATIC ${ANTLR_LIB_FILES})
target_link_libraries(compiler antlr Threads::Threads)

enable_testing()

add_test(
        NAME syntax_test
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/testbench/syntax_test.sh
)

add_test(
        NAME semantic_test
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/testbench/semantic_test.sh
)

add_test(
        NAME semantic_interpret_test
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/testbench/semantic_interpret_test.sh
)

add_test(
        NAME codegen_test
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/testbench/codegen_test.sh
)

add_test(
        NAME codegen_test_against
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/testbench/codegen_test_against.sh
)

add_test(
        NAME codegen_count
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/testbench/codegen_count.sh
)

add_test(
        NAME codegen_test_functional_against
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/testbench/codegen_test_functional_against.sh
)

set_tests_properties(syntax_test PROPERTIES TIMEOUT 10)
set_tests_properties(semantic_test PROPERTIES TIMEOUT 60)
set_tests_properties(semantic_interpret_test PROPERTIES TIMEOUT 20)
set_tests_properties(codegen_test PROPERTIES TIMEOUT 60)
set_tests_properties(codegen_test_against PROPERTIES TIMEOUT 400)
set_tests_properties(codegen_test_functional_against PROPERTIES TIMEOUT 60)