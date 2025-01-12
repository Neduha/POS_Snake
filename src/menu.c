#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/menu.h"

void clear_console_menu() {
    printf("\033[2J\033[H");
    fflush(stdout);
}

int handle_pause_menu() {
    clear_console_menu();
fprintf(stderr, "Pause menu called\n");
    const char *opts[] = {
        "Continue current game",
        "Start new game",
        "Quit"
    };
    for (int i = 0; i < 3; i++) {
        printf("%d. %s\n", i + 1, opts[i]);
    }
    int c = 0;
    do {
        printf("Enter choice (1-3): ");
        if (scanf("%d", &c) != 1) {
            while (getchar() != '\n');
            c = 0;
        }
    } while (c < 1 || c > 3);
    return c;
}

int handle_dead_menu() {
    clear_console_menu();
    const char *opts[] = {
        "Join again",
        "Start new game",
        "Quit"
    };
    for (int i = 0; i < 3; i++) {
        printf("%d. %s\n", i + 1, opts[i]);
    }
    int c = 0;
    do {
        printf("Enter choice (1-3): ");
        if (scanf("%d", &c) != 1) {
            while (getchar() != '\n');
            c = 0;
        }
    } while (c < 1 || c > 3);
    return c;
}

void menu_display(Menu *self) {
    printf("%s\n", self->title);
    for (int i = 0; i < self->option_count; i++) {
        printf("%d. %s\n", i + 1, self->options[i]);
    }
}

int menu_get_choice(Menu *self) {
    int choice;

    do {
        printf("Enter your choice (1-%d): ", self->option_count);
        if (scanf("%d", &choice) != 1 || choice < 1 || choice > self->option_count) {
            while (getchar() != '\n');
            choice = 0;
        }
    } while (choice < 1 || choice > self->option_count);

    return choice;
}

Menu create_menu(const char *title, const char *options[], int option_count) {
    Menu menu;
    menu.title = strdup(title);
    menu.option_count = option_count;
    for (int i = 0; i < option_count; i++) {
        menu.options[i] = strdup(options[i]);
    }
    menu.display = menu_display;
    menu.get_choice = menu_get_choice;
    return menu;
}

void destroy_menu(Menu *menu) {
    free(menu->title);
    for (int i = 0; i < menu->option_count; i++) {
        free(menu->options[i]);
    }
}

int handle_main_menu() {
    const char *menu_options[] = {
        "Start a new game",
        "Join an ongoing game",
        "Continue a paused game",
        "Quit"
    };

    Menu main_menu = create_menu("Main Menu", menu_options, 4);
    main_menu.display(&main_menu);
    int choice = main_menu.get_choice(&main_menu);
    destroy_menu(&main_menu);
    return choice;
}

int handle_game_mode() {
    const char *game_modes[] = {
        "Time-based game",
        "Standard game",
        "Quit game setup"
    };

    Menu game_mode_menu = create_menu("Choose Game Mode", game_modes, 3);
    game_mode_menu.display(&game_mode_menu);
    int choice = game_mode_menu.get_choice(&game_mode_menu);
    destroy_menu(&game_mode_menu);

    return choice;
}

int handle_map_type() {
    const char *map_types[] = {
        "Clean map",
        "Map with obstacles",
        "Quit game setup"
    };

    Menu map_type_menu = create_menu("Choose Map Type", map_types, 3);
    map_type_menu.display(&map_type_menu);
    int choice = map_type_menu.get_choice(&map_type_menu);
    destroy_menu(&map_type_menu);

    return choice;
}

int get_game_duration() {
    int minutes;

    do {
        printf("Enter the duration of the game in minutes (1-60): ");
        if (scanf("%d", &minutes) != 1 || minutes < 1 || minutes > 60) {
            while (getchar() != '\n');
            minutes = 0;
        }
    } while (minutes < 1 || minutes > 60);

    return minutes;
}

int get_grid_size() {
    int grid_size;

    do {
        printf("Enter grid size (5-50): ");
        if (scanf("%d", &grid_size) != 1 || grid_size < 5 || grid_size > 50) {
            while (getchar() != '\n');
            grid_size = 0;
        }
    } while (grid_size < 5 || grid_size > 50);

    return grid_size;
}

