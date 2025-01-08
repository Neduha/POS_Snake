
cmake_minimum_required(VERSION 3.10)
project(SnakeGame C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(client
    src/client.c
    src/snake_game.c
    src/pipe.c
)

add_executable(server
    src/server.c
    src/snake_game.c
    src/pipe.c
)

target_link_libraries(client pthread)
target_link_libraries(server pthread)
target_link_libraries(client ncurses)
target_link_libraries(server ncurses)
