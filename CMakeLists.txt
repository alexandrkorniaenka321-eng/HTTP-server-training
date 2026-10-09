cmake_minimum_required(VERSION 3.10)

project("MyApp")

set(CMAKE_CXX_STANDARD 20)

add_executable(http_server
    main.cpp
    HTTPRequest.cpp
    HTTPResponce.cpp
)