#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <ncurses.h>
#include <signal.h>
#include "../include/pipe.h"
#include "../include/snake_game.h"
#include "../include/menu.h"

#define FIFO_PATH "client_to_server_fifo"
#define FIFO_PATH1 "server_to_client_fifo"
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

int fd_write;
int fd_read;
int direction = RIGHT;
static pthread_mutex_t ncurses_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t send_thread, receive_thread;
int restart_requested = 0;
int quit_requested = 0;

void handle_input(int *direction) {
    int ch = getch();
    if (ch == 'p') {
        int pause_cmd = -2;
        pipe_write_fd(fd_write, &pause_cmd, sizeof(int));
    } else if (ch == KEY_UP && *direction != DOWN) {
        *direction = UP;
    } else if (ch == KEY_RIGHT && *direction != LEFT) {
        *direction = RIGHT;
    } else if (ch == KEY_DOWN && *direction != UP) {
        *direction = DOWN;
    } else if (ch == KEY_LEFT && *direction != RIGHT) {
        *direction = LEFT;
    }
}

void* send_directives(void* arg) {
    int last_sent_direction = -1;
    while (1) {
        pthread_mutex_lock(&ncurses_mutex);
        handle_input(&direction);
        pthread_mutex_unlock(&ncurses_mutex);

        if (direction != last_sent_direction && direction >= 0) {
            pipe_write_fd(fd_write, &direction, sizeof(int));
            last_sent_direction = direction;
        }
        usleep(100000);
    }
    return NULL;
}

void* receive_game_state(void* arg) {
    GameStateData data;
    static int requested_join = 0;
    static int waiting_for_unpause = 0;
    while (1) {
        ssize_t r = read(fd_read, &data, sizeof(data));
        if (r == 0) {
            break;
        } else if (r < 0) {
            continue;
        } else if (r < (ssize_t)sizeof(data)) {
            break;
        }

        pthread_mutex_lock(&ncurses_mutex);
        render_game_from_data(&data);

        if (data.paused && !waiting_for_unpause){
            endwin() ;

            int choice = handle_pause_menu();
            if (choice == 3) {
                quit_requested = 1;
                pthread_mutex_unlock(&ncurses_mutex);
                break;
            }
            if (choice == 2) {
                restart_requested = 1;
                pthread_mutex_unlock(&ncurses_mutex);
                break;
            }
            if (choice == 1) {
                int unpause_cmd = -3;
                pipe_write_fd(fd_write, &unpause_cmd, sizeof(int));
                waiting_for_unpause = 1;
                pthread_mutex_unlock(&ncurses_mutex);
                continue;
            }
            initscr();
            cbreak();
            noecho();
            keypad(stdscr, TRUE);
            nodelay(stdscr, TRUE);
            curs_set(0);
        }

        if (data.dead && !requested_join) {
            endwin();
            int c = handle_dead_menu();
            if (c == 3) {
                quit_requested = 1;
                pthread_mutex_unlock(&ncurses_mutex);
                break;
            }
            if (c == 2) {
                restart_requested = 1;
                pthread_mutex_unlock(&ncurses_mutex);
                break;
            }
            initscr();
            cbreak();
            noecho();
            keypad(stdscr, TRUE);
            nodelay(stdscr, TRUE);
            curs_set(0);
            if (c == 1) {
                requested_join = 1;
                int join_cmd = -5;
                pipe_write_fd(fd_write, &join_cmd, sizeof(int));
            }
        }

        if (!data.paused) {
            waiting_for_unpause = 0;
        }
        if (!data.dead) {
            requested_join = 0;
        }

        pthread_mutex_unlock(&ncurses_mutex);
        usleep(50000);
    }

    endwin();
    pthread_cancel(send_thread);
    pthread_exit(NULL);
    return NULL;
}

int main(int argc, char *argv[]) {
    signal(SIGPIPE, SIG_IGN);
    setvbuf(stdout, NULL, _IONBF, 0);

    do {
        restart_requested = 0;
        quit_requested = 0;
        const char *menu_options[] = {
            "Start a new game",
            "Join an ongoing game",
            "Continue a paused game",
            "Quit"
        };
        Menu menu = create_menu("Main Menu", menu_options, 4);
        menu.display(&menu);
        int choice = menu.get_choice(&menu);

        switch (choice) {
            case 1: {
                int gm = handle_game_mode();
                int duration_minutes = 0;
                if (gm == 1) {
                    duration_minutes = get_game_duration();
                } else if (gm == 3) {
                    destroy_menu(&menu);
                    break;
                }
                int map_type = handle_map_type();
                int obstacles_flag = (map_type == 2) ? 1 : 0;
                if (map_type == 3) {
                    destroy_menu(&menu);
                    break;
                }
                int grid_size = get_grid_size();
                pid_t pid = fork();
                if (pid < 0) {
                    destroy_menu(&menu);
                    return 1;
                }
                if (pid == 0) {
                    char gs[32];
                    char dm[32];
                    char of[32];
                    char mm[32];
                    snprintf(gs, sizeof(gs), "%d", grid_size);
                    snprintf(dm, sizeof(dm), "%d", duration_minutes);
                    snprintf(of, sizeof(of), "%d", obstacles_flag);
                    snprintf(mm, sizeof(mm), "%d", (gm == 1 ? 1 : 2));
                    execl("./server", "./server", gs, dm, of, mm, (char *)NULL);
                    exit(1);
                }
                sleep(1);
                fd_write = pipe_open_write(FIFO_PATH);
                fd_read = pipe_open_read(FIFO_PATH1);
                initscr();
                cbreak();
                noecho();
                keypad(stdscr, TRUE);
                nodelay(stdscr, TRUE);
                curs_set(0);
                pthread_create(&send_thread, NULL, send_directives, NULL);
                pthread_create(&receive_thread, NULL, receive_game_state, NULL);
                pthread_join(receive_thread, NULL);
                pthread_cancel(send_thread);
                pthread_join(send_thread, NULL);
                endwin();
                close(fd_write);
                close(fd_read);
                destroy_menu(&menu);
                break;
            }
            case 2:
            case 3:
            case 4:
                destroy_menu(&menu);
                return 0;
            default:
                break;
        }
    } while(restart_requested && !quit_requested);

    return 0;
}

