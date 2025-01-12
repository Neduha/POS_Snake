#ifndef MENU_H
#define MENU_H

#define MAX_MENU_OPTIONS 10

typedef struct Menu {
    char *title;
    int option_count;
    char *options[MAX_MENU_OPTIONS];
    void (*display)(struct Menu *self);
    int (*get_choice)(struct Menu *self);
} Menu;

void clear_console_menu();
int handle_pause_menu();
int handle_dead_menu();

void menu_display(Menu *self);
int menu_get_choice(Menu *self);

Menu create_menu(const char *title, const char *options[], int option_count);
void destroy_menu(Menu *menu);

int handle_main_menu();
int handle_game_mode();
int handle_map_type();
int get_game_duration();
int get_grid_size();

#endif

