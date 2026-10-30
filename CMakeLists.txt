cmake_minimum_required(VERSION 3.16)

project(easy_db LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(easy_db 
    src/common/status.cc
    src/database/row.cc
    src/page/page.cc
    src/page/page_manager.cc
    src/btree/node.cc
    src/btree/leaf_node.cc
   src/btree/internal_node.cc
)

target_include_directories(easy_db
    PUBLIC
        ${PROJECT_SOURCE_DIR}/src
)

enable_testing()

find_package(CppUTest REQUIRED)

add_executable(easy_db_test
    tests/main.cc
    tests/common/status_test.cc
    tests/common/result_test.cc
    tests/database/row_test.cc
    tests/page/page_test.cc
    tests/page/page_manager_test.cc
    tests/btree/node_test.cc
   tests/btree/leaf_node_test.cc
   tests/btree/internal_node_test.cc
)

target_link_libraries(easy_db_test
    PRIVATE
        easy_db
        CppUTest
)

add_test(
    NAME easy_db_test
    COMMAND easy_db_test
)