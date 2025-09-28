cmake_minimum_required(VERSION 3.16)
project(SimpleChat LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)

add_executable(SimpleChat
    src/main.cpp
    src/ChatWindow.cpp
    src/ChatWindow.h
    src/RingNode.cpp
    src/RingNode.h
)

target_link_libraries(SimpleChat
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
)
add_executable(SimpleInjector
    src/Injector.cpp
)
target_link_libraries(SimpleInjector
    Qt6::Core
    Qt6::Network
)
