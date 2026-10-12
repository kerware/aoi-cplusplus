# Dernière génération/modification faite par l'IA Claude le 12/10/2026 23:29:41
# Mini-projet de vérification : mêmes réglages que le projet fil rouge (C++20, MSVC, /W4 /WX).
cmake_minimum_required(VERSION 3.25)
project(verification_cpp20 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(verification_cpp20 main.cpp)
if(MSVC)
    target_compile_options(verification_cpp20 PRIVATE /W4 /WX /utf-8 /permissive- /Zc:__cplusplus)
endif()

enable_testing()
add_test(NAME cpp20 COMMAND verification_cpp20)
