#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

struct window_size {
    int row;
    int col;
};

int **create_grid(const struct window_size *wsize) {
    int **grid_addr = (int **)malloc(wsize->row * sizeof(int *));
    for (int r = 0; r < wsize->row; r++)
        grid_addr[r] = (int *)malloc(wsize->col * sizeof(int));
    return grid_addr;
}

void setup_grid(int **grid, const struct window_size *wsize) {
    for (int i = 0; i < wsize->row; i++)
        for (int j = 0; j < wsize->col; j++)
            grid[i][j] = rand() % 2;
}

void draw_grid(int **grid, const struct window_size *wsize) {
    for (int i = 1; i < wsize->row + 1; i++) {
        for (int j = 0; j < wsize->col; j++) {
            mvaddstr(i, j * 3, grid[i][j] == 1 ? " X " : "  ");
            refresh();
        }
    }
}

void draw_header(const struct window_size *wsize, int gen, double simulation_time, double last_draw_time) {
    char str[wsize->col];
    sprintf(str, "Generation : %d", gen);
    mvaddstr(0, 5, str);
    sprintf(str, "Simulation time : %fms", simulation_time);
    mvaddstr(0, wsize->col / 2 - 15, str);
    sprintf(str, "Draw time : %fms", last_draw_time);
    mvaddstr(0, wsize->col - 30, str);
}

int get_neighbours_count(int **grid, int l, int c, const struct window_size *wsize) {
    int count = 0;
    for (int i = l - 1; i <= l + 1; i++) {
        for (int j = c - 1; j <= c + 1; j++) {
            if (i == l && j == c)
                continue;
            if (i >= 0 && i < wsize->row && j >= 0 && j < wsize->col)
                count += grid[i][j];
        }
    }
    return count;
}

void simulate(int **grid, const struct window_size *wsize) {
    int **tmp = create_grid(wsize);
    for (int i = 0; i < wsize->row; i++) {
        for (int j = 0; j < wsize->col; j++) {
            tmp[i][j] = grid[i][j];
        }
    }
    for (int i = 0; i < wsize->row; i++) {
        for (int j = 0; j < wsize->col; j++) {
            int neighbour_count = get_neighbours_count(tmp, i, j, wsize);
            if (tmp[i][j]) {
                if (neighbour_count < 2 || neighbour_count > 3)
                    grid[i][j] = 0;
            } else if (neighbour_count == 3) {
                grid[i][j] = 1;
            }
        }
    }
}

int main() {
    initscr();
    curs_set(FALSE);

    int max_y = 0, max_x = 0;

    getmaxyx(stdscr, max_y, max_x);
    struct window_size wsize = {max_y, max_x};

    int **grid = create_grid(&wsize);
    setup_grid(grid, &wsize);

    int gen = 1;
    draw_grid(grid, &wsize);

    double last_draw_time = 0;
    while (1) {
        clock_t begin = clock();
        simulate(grid, &wsize);
        clock_t end = clock();
        double simulation_time = (double)(end - begin) / CLOCKS_PER_SEC;

        begin = clock();
        draw_grid(grid, &wsize);
        end = clock();
        last_draw_time = (double)(end - begin) / CLOCKS_PER_SEC;

        draw_header(&wsize, gen++, simulation_time, last_draw_time);
    }
    endwin();
    return 0;
}
