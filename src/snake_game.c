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
    state->direction = RIGHT;
    srand(time(NULL));
}

void render_game_from_data(GameStateData *data) {
  
  clear();
    for (int y = 0; y < data->height + 2; y++) {
        for (int x = 0; x < data->width + 2; x++) {
            if (y == 0 || y == data->height + 1 || x == 0 || x == data->width + 1) {
                mvprintw(y, x, "#");
            } else if (x == data->snake_body[0].x + 1 && y == data->snake_body[0].y + 1) {
                mvprintw(y, x, "O");
            } else if (x == data->fruit.x + 1 && y == data->fruit.y + 1) {
                mvprintw(y, x, "*");
            } else {
                int is_body = 0;
                for (int i = 1; i < data->snake_length; i++) {
                    if (x == data->snake_body[i].x + 1 && y == data->snake_body[i].y + 1) {
                        mvprintw(y, x, "o");
                        is_body = 1;
                        break;
                    }
                }
                if (!is_body) mvprintw(y, x, " ");
            }
        }
    }
    mvprintw(data->height + 3, 0, "Score: %d\n", data->score);
    refresh();
}

