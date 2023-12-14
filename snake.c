#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define board_height 35
#define board_width 90

struct List {
    int pos_x;
    int pos_y;
    struct List *next;
};

int get_food_x() {
    int x = rand() % board_width;
    if (x % 2) {
        x++;
        x %= board_width;
    }
    return x;
}

int get_food_y() {
    return rand() % board_height;
}

bool check_food_on_snake(struct List *positions, int food_x, int food_y) {
    struct List *it = positions;
    while (it) {
        if (food_x == it->pos_x && food_y == it->pos_y) {
            return true;
        }
        it = it->next;
    }
    return false;
}

void draw_borders() {
    char lower[board_width] = {[0 ... board_width-2] = '-'};
    mvaddstr(board_height, 0, lower);
    for (int y = 0; y < board_height; y++) {
        mvaddch(y, board_width, '|');
    }
}

void draw_snake(struct List *positions) {
    struct List *it = positions;
    while (it) {
        mvaddch(it->pos_y, it->pos_x, '*');
        it = it->next;
    }
}


int main() {
    initscr();
    cbreak();
    noecho();
    intrflush(stdscr, false);
    keypad(stdscr, true);
    nodelay(stdscr, true);

    struct List *positions = (struct List*)malloc(sizeof(struct List));
    positions->pos_x = 0;
    positions->pos_y = 0;
    positions->next = NULL;

    int dir_x = 0;
    int dir_y = 0;

    int food_x = get_food_x();
    int food_y = get_food_y();

    while (true) {
        int pressed = getch();
        if (pressed == KEY_LEFT && dir_x == 0) {
            dir_x = -2;
            dir_y = 0;
        }
        if (pressed == KEY_RIGHT && dir_x == 0) {
            dir_x = 2;
            dir_y = 0;
        }
        if (pressed == KEY_UP && dir_y == 0) {
            dir_x = 0;
            dir_y = -1;
        }
        if (pressed == KEY_DOWN && dir_y == 0) {
            dir_x = 0;
            dir_y = 1;
        }

        // update position
        struct List *new_pos = (struct List*)malloc(sizeof(struct List));
        new_pos->pos_x = positions->pos_x + dir_x + board_width;
        new_pos->pos_y = positions->pos_y + dir_y + board_height;
        new_pos->next = positions;
        new_pos->pos_x %= board_width;
        new_pos->pos_y %= board_height;

        positions = new_pos;

        const int current_x = positions->pos_x;
        const int current_y = positions->pos_y;

        // look if we hit food
        if (current_x == food_x && current_y == food_y) {
            food_x = get_food_x();
            food_y = get_food_y();
            while (check_food_on_snake(positions, food_x, food_y)) {
                food_x = get_food_x();
                food_y = get_food_y();
            }
        } else {
            struct List *it = positions;
            while (it->next->next) {
                it = it->next;
            }
            free(it->next);
            it->next = NULL;
        }

        // look for game over
        struct List *it = positions->next;
        int length = 1;
        while (it) {
            if (current_x == it->pos_x && current_y == it->pos_y) {
                while (it) {
                    length++;
                    it = it->next;
                }
                char snake_length_str[50] = "SNAKE LENGTH: ";
                char str[30];
                snprintf(str, 30, "%d", length);
                strcat(snake_length_str, str);
                mvaddstr(board_height/2-3, board_width/2-7, "GAME OVER");
                mvaddstr(board_height/2-1, board_width/2-10, snake_length_str);
                refresh();
                usleep(3e6);
                goto end;
            }
            length++;
            it = it->next;
        }

        // draw everything again
        erase();
        draw_borders();
        draw_snake(positions);
        mvaddch(food_y, food_x, '@');

        usleep(50e3);
    }

end:
    while (positions) {
        struct List *next = positions->next;
        free(positions);
        positions = next;
    }

    endwin();
    return 0;
}
