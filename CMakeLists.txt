
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



# Biblioteca compartida de red
add_library(SpaceGangShared STATIC
    src/shared/protocol/Protocol.cpp
    src/shared/network/TcpConnection.cpp
)

target_include_directories(SpaceGangShared PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/shared
)

target_link_libraries(SpaceGangShared PUBLIC
    SFML::Network
)

# Cliente


add_executable(SpaceGangClient
    src/client/main.cpp
    src/client/game/Game.cpp
    src/client/ui/Button.cpp
    src/client/network/NetworkClient.cpp
    src/client/lobby/ClientLobby.cpp
)

target_include_directories(SpaceGangClient PRIVATE
    src/client
)

target_link_libraries(SpaceGangClient PRIVATE
    SpaceGangShared
    SFML::Graphics
)



# Servidor
add_executable(SpaceGangServer
    src/server/main.cpp
    src/server/GameServer.cpp
    src/server/lobby/LobbyManager.cpp
)

target_include_directories(SpaceGangServer PRIVATE
    src/server
)

target_link_libraries(SpaceGangServer PRIVATE
    SpaceGangShared
)

# Prueba del protocolo
add_executable(ProtocolTest
    tests/ProtocolTest.cpp
)

target_link_libraries(ProtocolTest PRIVATE
    SpaceGangShared
)


add_custom_command(
    TARGET SpaceGangClient POST_BUILD

    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:SpaceGangClient>/assets"

    COMMENT "Copiando recursos de SpaceGang"
)

