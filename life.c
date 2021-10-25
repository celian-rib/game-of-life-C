#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

struct window_size {
    int row;
    int col;
};

int **GRID;
struct window_size wsize;

int **create_GRID() {
    int **GRID_addr = (int **)malloc(wsize.row * sizeof(int *));
    for (int r = 0; r < wsize.row; r++)
        GRID_addr[r] = (int *)malloc(wsize.col * sizeof(int));
    return GRID_addr;
}

void setup_GRID() {
    for (int i = 0; i < wsize.row; i++)
        for (int j = 0; j < wsize.col; j++)
            GRID[i][j] = rand() % 2;
}

void draw_GRID() {
    for (int i = 1; i < wsize.row + 1; i++) {
        for (int j = 0; j < wsize.col; j++) {
            mvaddstr(i, j * 3, GRID[i][j] == 1 ? " X " : "  ");
            refresh();
        }
    }
}

void draw_header(int gen, double simulation_time, double last_draw_time) {
    char str[wsize.col];
    sprintf(str, "Generation : %d", gen);
    mvaddstr(0, 5, str);
    sprintf(str, "Simulation time : %fms", simulation_time);
    mvaddstr(0, wsize.col / 2 - 15, str);
    sprintf(str, "Draw time : %fms", last_draw_time);
    mvaddstr(0, wsize.col - 30, str);
}

int get_neighbours_count(int **GRID, int l, int c) {
    int count = 0;
    for (int i = l - 1; i <= l + 1; i++) {
        for (int j = c - 1; j <= c + 1; j++) {
            if (i == l && j == c)
                continue;
            if (i >= 0 && i < wsize.row && j >= 0 && j < wsize.col)
                count += GRID[i][j];
        }
    }
    return count;
}

void simulate() {
    int **tmp = create_GRID();
    for (int i = 0; i < wsize.row; i++) {
        for (int j = 0; j < wsize.col; j++) {
            tmp[i][j] = GRID[i][j];
        }
    }
    for (int i = 0; i < wsize.row; i++) {
        for (int j = 0; j < wsize.col; j++) {
            int neighbour_count = get_neighbours_count(tmp, i, j);
            if (tmp[i][j]) {
                if (neighbour_count < 2 || neighbour_count > 3)
                    GRID[i][j] = 0;
            } else if (neighbour_count == 3) {
                GRID[i][j] = 1;
            }
        }
    }
    free(tmp);
}

int main() {
    initscr();
    curs_set(FALSE);

    int max_y = 0, max_x = 0;

    getmaxyx(stdscr, max_y, max_x);
    wsize.col = max_x;
    wsize.row = max_y;

    GRID = create_GRID();
    setup_GRID();

    int gen = 1;
    draw_GRID();

    double last_draw_time = 0;
    while (1) {
        clock_t begin = clock();
        simulate();
        clock_t end = clock();
        double simulation_time = (double)(end - begin) / CLOCKS_PER_SEC;

        begin = clock();
        draw_GRID();
        end = clock();
        last_draw_time = (double)(end - begin) / CLOCKS_PER_SEC;

        draw_header(gen++, simulation_time, last_draw_time);
    }
    endwin();
    return 0;
}
