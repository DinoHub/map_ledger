cmake_minimum_required(VERSION 3.4)

project(
    map_ledger
    VERSION 1.0
    LANGUAGES CXX
)

set(OPENSSL_USE_STATIC_LIBS TRUE)

find_package(OpenSSL REQUIRED)

set(CMAKE_CXX_STANDARD 11)

include_directories(
    include
)

add_executable(
    map_ledger_demo
    src/ledger.cpp
    src/base64.cpp
)

target_link_libraries(map_ledger_demo PRIVATE OpenSSL::Crypto)

install(TARGETS map_ledger_demo DESTINATION ${INSTALL_BIN_DIR})