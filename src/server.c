#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../include/pipe.h"
#include "../include/snake_game.h"

#define FIFO_PATH "client_to_server_fifo"
#define FIFO_PATH1 "server_to_client_fifo"

void init_server(int grid_height);

int main(int argc, char *argv[]) {
    if (argc != 2) {

        return 1;
    }

    int grid_height = atoi(argv[1]);
    if (grid_height < 5 || grid_height > 50) {
        printf("Invalid grid height. Please enter a number between 5 and 50.\n");
        return 1;
    }

    pipe_init(FIFO_PATH);
    pipe_init(FIFO_PATH1);
    
    pipe_flush(FIFO_PATH);
    pipe_flush(FIFO_PATH1);  


    GameState state;
    init_game(&state, grid_height);
    int last_direction = RIGHT;  

    int fd_read  = pipe_open_read(FIFO_PATH);   
    int fd_write = pipe_open_write(FIFO_PATH1);


    while (1) {
        int new_direction = last_direction;
        pipe_read_fd(fd_read, &new_direction, sizeof(int));
        
        last_direction = new_direction;

        state.direction = new_direction;

      
        Position head = state.snake.body[0];
        if (state.direction == UP) head.y -= 1;
        else if (state.direction == RIGHT) head.x += 1;
        else if (state.direction == DOWN) head.y += 1;
        else if (state.direction == LEFT) head.x -= 1;

        if (head.x < 0) head.x = state.width - 1;
        else if (head.x >= state.width) head.x = 0;
        if (head.y < 0) head.y = state.height - 1;
        else if (head.y >= state.height) head.y = 0;

        for (int i = state.snake.length - 1; i > 0; i--) {
            state.snake.body[i] = state.snake.body[i - 1];
        }
        state.snake.body[0] = head;

        if (head.x == state.fruit.x && head.y == state.fruit.y) {
            state.score += 10;
            state.snake.length++;
            state.fruit.x = rand() % state.width;
            state.fruit.y = rand() % state.height;
        }

      
        GameStateData data = {
            .width = state.width,
            .height = state.height,
            .score = state.score,
            .fruit = state.fruit,
            .snake_length = state.snake.length,
        };
        for (int i = 0; i < state.snake.length; i++) {
            data.snake_body[i] = state.snake.body[i];
        }


      
        pipe_write_fd(fd_write, &data, sizeof(data)); 

        usleep(90000);
    }

    return 0;
}

