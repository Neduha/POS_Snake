#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ncurses.h>
#include "../include/pipe.h"
#include "../include/snake_game.h"

#define FIFO_PATH "client_to_server_fifo"
#define FIFO_PATH1 "server_to_client_fifo"
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

void start_server(int grid_size);
void handle_input(int *direction);

int main(int argc, char *argv[]) {

    printf("Client started");

    if (argc != 2) {
        printf("Usage: %s <grid_size>\n", argv[0]);
        return 1;
    }

    int grid_size = atoi(argv[1]);
    if (grid_size < 5 || grid_size > 100) {
        printf("Invalid grid size. Please enter a number between 5 and 100.\n");
        return 1;
    }

  
        pid_t pid = fork();
        if (pid < 0) {
            perror("Failed to fork process for server");
            return 1;
        }
        if (pid == 0) {
            start_server(grid_size);
            exit(0);
        }

        sleep(1);
   

        int fd_write = pipe_open_write(FIFO_PATH);   
        int fd_read  = pipe_open_read(FIFO_PATH1);    


        int direction = RIGHT;
        initscr();
        cbreak();
        noecho();
        keypad(stdscr, TRUE);
        nodelay(stdscr, TRUE);
        curs_set(0);

        while (1) {
            handle_input(&direction);
        
            pipe_write_fd(fd_write, &direction, sizeof(direction)); 

            GameStateData data;
            pipe_read_fd(fd_read, &data, sizeof(data));

            render_game_from_data(&data);
            
            usleep(120000);
        }

        endwin();
        return 0;
    }

void start_server(int grid_size) {

    char grid_size_str[10];
    sprintf(grid_size_str, "%d", grid_size);
    execl("./server", "./server", grid_size_str, (char *)NULL);

    perror("Failed to start server");
    exit(1);
}

void handle_input(int *direction) {
    int ch = getch();
    if (ch == KEY_UP && *direction != DOWN) {
        *direction = UP;
    } else if (ch == KEY_RIGHT && *direction != LEFT) {
        *direction = RIGHT;
    } else if (ch == KEY_DOWN && *direction != UP) {
        *direction = DOWN;
    } else if (ch == KEY_LEFT && *direction != RIGHT) {
        *direction = LEFT;
    }
}

