cmake_minimum_required(VERSION 3.10)
project(SnakeGame C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=address")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fsanitize=address")

include_directories(include)

add_executable(client
    src/client.c
    src/menu.c
    src/snake_game.c
    src/pipe.c
)

add_executable(server
    src/server.c
    src/snake_game.c
    src/pipe.c
)

target_link_libraries(client pthread ncurses)
target_link_libraries(server pthread ncurses)

