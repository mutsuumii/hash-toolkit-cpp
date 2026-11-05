cmake_minimum_required(VERSION 3.16)
project(hash_toolkit LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(hash_toolkit src/main.cpp)

find_package(OpenSSL REQUIRED)
target_link_libraries(hash_toolkit OpenSSL::Crypto)
