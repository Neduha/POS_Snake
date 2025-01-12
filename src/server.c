#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include "../include/pipe.h"
#include "../include/snake_game.h"

#define FIFO_PATH "client_to_server_fifo"
#define FIFO_PATH1 "server_to_client_fifo"
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3
#define MAX_OBSTACLES 200

int snake_collides(GameState* state) {
    Position head = state->snake.body[0];
    for (int i = 1; i < state->snake.length; i++) {
        if (head.x == state->snake.body[i].x && head.y == state->snake.body[i].y) {
            return 1;
        }
    }
    return 0;
}

int obstacle_collides(GameState* state) {
    if (!state->obstacles_flag) return 0;
    Position head = state->snake.body[0];
    for (int i = 0; i < state->obstacles_count; i++) {
        if (head.x == state->obstacles[i].x && head.y == state->obstacles[i].y) {
            return 1;
        }
    }
    return 0;
}

void place_obstacles(GameState* state) {
    int base = (state->width * state->height) / 50;
    if (base < 5) base = 5;
    if (base > MAX_OBSTACLES) base = MAX_OBSTACLES;
    state->obstacles_count = base;
    int placed = 0;
    while (placed < state->obstacles_count) {
        Position pos;
        pos.x = rand() % state->width;
        pos.y = rand() % state->height;
        int conflict = 0;
        for (int i = 0; i < state->snake.length; i++) {
            if (pos.x == state->snake.body[i].x && pos.y == state->snake.body[i].y) {
                conflict = 1;
                break;
            }
        }
        for (int i = 0; i < placed && !conflict; i++) {
            if (pos.x == state->obstacles[i].x && pos.y == state->obstacles[i].y) {
                conflict = 1;
                break;
            }
        }
        if (!conflict && pos.x != state->fruit.x && pos.y != state->fruit.y) {
            state->obstacles[placed] = pos;
            placed++;
        }
    }
}

void spawn_fruit(GameState* state) {
    while (1) {
        Position f;
        f.x = rand() % state->width;
        f.y = rand() % state->height;
        int conflict = 0;
        for (int i = 0; i < state->snake.length; i++) {
            if (f.x == state->snake.body[i].x && f.y == state->snake.body[i].y) {
                conflict = 1;
                break;
            }
        }
        if (!conflict && state->obstacles_flag) {
            for (int i = 0; i < state->obstacles_count; i++) {
                if (f.x == state->obstacles[i].x && f.y == state->obstacles[i].y) {
                    conflict = 1;
                    break;
                }
            }
        }
        if (!conflict) {
            state->fruit = f;
            break;
        }
    }
}

void reset_game(GameState* state) {
    state->snake.length = 1;
    state->snake.body[0].x = state->width / 2;
    state->snake.body[0].y = state->height / 2;
    state->direction = RIGHT;
    state->paused = 0;
    state->dead = 0;
    spawn_fruit(state);
    if (state->obstacles_flag) {
        place_obstacles(state);
    }
}

void reset_player(GameState* state) {
    state->snake.length = 1;
    state->snake.body[0].x = state->width / 2;
    state->snake.body[0].y = state->height / 2;
    state->score = 0;
    state->dead = 0;
}

int main(int argc, char *argv[]) {
    signal(SIGPIPE, SIG_IGN);
    if (argc != 5) {
        return 1;
    }

    int grid_height = atoi(argv[1]);
    int duration_minutes = atoi(argv[2]);
    int obstacles_flag = atoi(argv[3]);
    int mode = atoi(argv[4]);
    int duration_seconds = duration_minutes * 60;

    pipe_init(FIFO_PATH);
    pipe_init(FIFO_PATH1);
    pipe_flush(FIFO_PATH);
    pipe_flush(FIFO_PATH1);

    srand(time(NULL));

    GameState state;
    init_game(&state, grid_height);
    state.obstacles_flag = obstacles_flag;
    state.game_mode = mode;

    if (obstacles_flag) {
        place_obstacles(&state);
    }

    spawn_fruit(&state);

    time_t start_time = time(NULL);
    time_t last_client_activity = time(NULL);

    int fd_read = 0, fd_write = 0;
    int last_direction = state.direction;
    state.dead = 0;
    state.paused = 0;

    while (1) {
        if (!fd_read || !fd_write) {
            fd_read = open(FIFO_PATH, O_RDONLY | O_NONBLOCK);
            if (fd_read < 0) perror("fd_read open failed");
            fd_write = pipe_open_write(FIFO_PATH1);
        }

        if (state.game_mode == 1 && duration_seconds > 0 && !state.paused) {
            time_t now = time(NULL);
            if ((int)(now - start_time) >= duration_seconds) {
                break;
            }
            state.time_remaining = duration_seconds - (int)(now - start_time);
        }

        if (state.game_mode == 2) {
            time_t now = time(NULL);
            if (difftime(now, last_client_activity) >= 10) {
                break;
            }
        }

        int new_direction = last_direction;
        int read_any = 0;

        while (1) {
            int temp_dir;
            ssize_t bytesRead = read(fd_read, &temp_dir, sizeof(temp_dir));
            if (bytesRead == -1) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) break;
            } else if (bytesRead == 0) {
                break;
            } else {
                read_any = 1;
                last_client_activity = time(NULL);
                if (temp_dir == -2) {
                        state.paused = 1; 
                  }

                else if (temp_dir == -3) state.paused = 0;
                else if (temp_dir == -4) reset_game(&state);
                else if (temp_dir == -5) reset_player(&state);
                else if (temp_dir == -6) {
                    close(fd_read);
                    close(fd_write);
                    fd_read = 0;
                    fd_write = 0;
                    break;
                } else new_direction = temp_dir;
            }
        }

        if (!read_any && mode == 2) {
            time_t now = time(NULL);
            if (difftime(now, last_client_activity) >= 10) {
                break;
            }
        }

        if (state.paused || state.dead) {
            continue;
        }

        last_direction = new_direction;
        state.direction = new_direction;

        Position head = state.snake.body[0];

        if (state.direction == UP) head.y--;
        else if (state.direction == RIGHT) head.x++;
        else if (state.direction == DOWN) head.y++;
        else if (state.direction == LEFT) head.x--;

        if (head.x < 0) head.x = state.width - 1;
        else if (head.x >= state.width) head.x = 0;
        if (head.y < 0) head.y = state.height - 1;
        else if (head.y >= state.height) head.y = 0;

        for (int i = state.snake.length - 1; i > 0; i--) {
            state.snake.body[i] = state.snake.body[i - 1];
        }
        state.snake.body[0] = head;

        if (snake_collides(&state)) state.dead = 1;
        if (!state.dead && obstacle_collides(&state)) state.dead = 1;
        if (!state.dead && head.x == state.fruit.x && head.y == state.fruit.y) {
            state.score += 10;
            state.snake.length++;
            spawn_fruit(&state);
        }

        GameStateData data;
        data.width = state.width;
        data.height = state.height;
        data.score = state.score;
        data.fruit = state.fruit;
        data.snake_length = state.snake.length;
        data.time_remaining = state.time_remaining;
        data.obstacles_flag = state.obstacles_flag;
        data.obstacles_count = state.obstacles_count;
        data.game_mode = state.game_mode;
        data.paused = state.paused;
        data.dead = state.dead;

        for (int i = 0; i < state.snake.length; i++) {
            data.snake_body[i] = state.snake.body[i];
        }
        for (int i = 0; i < state.obstacles_count; i++) {
            data.obstacles[i] = state.obstacles[i];
        }

        if (fd_write) {
            pipe_write_fd(fd_write, &data, sizeof(data));
        }

        usleep(100000);
    }

    close(fd_read);
    close(fd_write);
    return 0;
}

