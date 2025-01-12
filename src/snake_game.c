#include <stdio.h>
#include <ncurses.h>
#include <time.h>
#include <stdlib.h>
#include "../include/snake_game.h"

void init_game(GameState *state, int height) {
    state->height = height;
    state->width = 2 * height;
    state->snake.length = 1;
    state->snake.body[0].x = state->width / 2;
    state->snake.body[0].y = state->height / 2;
    state->fruit.x = rand() % state->width;
    state->fruit.y = rand() % state->height;
    state->score = 0;
    state->direction = 1;
    state->time_remaining = 0;
    state->obstacles_flag = 0;
    state->obstacles_count = 0;
    state->game_mode = 1;
    state->paused = 0;
    state->client_active_timer = 0;
    state->dead = 0;
    srand(time(NULL));
}

void render_game_from_data(GameStateData *data) {
    clear();
    for (int y = 0; y < data->height + 2; y++) {
        for (int x = 0; x < data->width + 2; x++) {
            if (y == 0 || y == data->height + 1 || x == 0 || x == data->width + 1) {
                mvprintw(y, x, "#");
            } else {
                int hx = data->snake_body[0].x + 1;
                int hy = data->snake_body[0].y + 1;
                if (x == hx && y == hy) {
                    mvprintw(y, x, "O");
                } else {
                    int fx = data->fruit.x + 1;
                    int fy = data->fruit.y + 1;
                    if (x == fx && y == fy) {
                        mvprintw(y, x, "*");
                    } else {
                        int printed = 0;
                        for (int i = 1; i < data->snake_length; i++) {
                            int sx = data->snake_body[i].x + 1;
                            int sy = data->snake_body[i].y + 1;
                            if (x == sx && y == sy) {
                                mvprintw(y, x, "o");
                                printed = 1;
                                break;
                            }
                        }
                        if (!printed && data->obstacles_flag) {
                            for (int i = 0; i < data->obstacles_count; i++) {
                                int ox = data->obstacles[i].x + 1;
                                int oy = data->obstacles[i].y + 1;
                                if (x == ox && y == oy) {
                                    mvprintw(y, x, "X");
                                    printed = 1;
                                    break;
                                }
                            }
                        }
                        if (!printed) {
                            mvprintw(y, x, " ");
                        }
                    }
                }
            }
        }
    }
    mvprintw(data->height + 3, 0, "Score: %d", data->score);
    if (data->game_mode == 1) {
        mvprintw(data->height + 4, 0, "Time remaining: %d", data->time_remaining);
    }
    if (data->paused) {
        mvprintw(data->height + 5, 0, "[Paused]");
    }
    if (data->dead) {
        mvprintw(data->height + 6, 0, "[Dead]");
    }
    refresh();
}

