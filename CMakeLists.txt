
cmake_minimum_required(VERSION 3.21)

project(SpaceGang LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(SFML)


# CLIENTE
add_executable(SpaceGangClient
    src/client/main.cpp
    src/client/network/NetworkClient.cpp
)

target_include_directories(SpaceGangClient PRIVATE
    src/client
    src/shared
)

target_link_libraries(SpaceGangClient PRIVATE
    SFML::Graphics
    SFML::Network
)

# SERVIDOR

add_executable(SpaceGangServer
    src/server/main.cpp
    src/server/GameServer.cpp
    src/server/lobby/LobbyManager.cpp
)

target_include_directories(SpaceGangServer PRIVATE
    src/server
    src/shared
)

target_link_libraries(SpaceGangServer PRIVATE
    SFML::Network
)

