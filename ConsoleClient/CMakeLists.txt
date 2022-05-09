cmake_minimum_required(VERSION 3.22.1)

add_executable(
    ConsoleClient
    console_client.cpp
)

target_compile_options(ConsoleClient PRIVATE -DUNICODE -D_UNICODE)