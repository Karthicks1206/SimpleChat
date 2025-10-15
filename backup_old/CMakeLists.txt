cmake_minimum_required(VERSION 3.16)
project(SimpleChat2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 6.5 COMPONENTS Core Gui Widgets Network Test REQUIRED)

add_subdirectory(tests)

add_executable(simplechat2
  src/main.cpp
  src/UiWindow.h src/UiWindow.cpp
  src/ChatNode.h  src/ChatNode.cpp
  src/Message.h   src/Message.cpp
  src/Protocol.h  src/Protocol.cpp
  src/VectorClock.h src/VectorClock.cpp
  src/PeerDirectory.h src/PeerDirectory.cpp
)

target_link_libraries(simplechat2 PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Network)
