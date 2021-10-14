#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

int **create_grid(const struct winsize *wsize) {
    int **grid_addr = (int **)malloc(wsize->ws_row * sizeof(int *));
    for (int r = 0; r < wsize->ws_row; r++)
        grid_addr[r] = (int *)malloc(wsize->ws_col * sizeof(int));
    return grid_addr;
}

void setup_grid(int **grid, const struct winsize *wsize) {
    for (int i = 0; i < wsize->ws_row; i++)
        for (int j = 0; j < wsize->ws_col; j++)
            grid[i][j] = rand() % 2;
}

void draw_line(int *lineAddr, int width) {
    for (int i = 0; i < width; i++)
        printf("|%s", lineAddr[i] == 1 ? "X" : " ");
    printf("|\n");
}

void draw_grid(int **grid, const struct winsize *wsize, int gen) {
    system("clear");
    printf("Generation : %d\n", gen);
    for (int i = 0; i < wsize->ws_row; i++)
        draw_line(grid[i], wsize->ws_col);
}

int get_neighbours_count(int **grid, int l, int c, const struct winsize *wsize) {
    int count = 0;
    for (int i = l - 1; i <= l + 1; i++) {
        for (int j = c - 1; j <= c + 1; j++) {
            if (i == l && j == c)
                continue;
            if (i >= 0 && i < wsize->ws_row && j >= 0 && j < wsize->ws_col)
                count += grid[i][j];
        }
    }
    return count;
}

void simulate(int **grid, const struct winsize *wsize) {
    int **tmp = create_grid(wsize);
    for (int i = 0; i < wsize->ws_row; i++) {
        for (int j = 0; j < wsize->ws_col; j++) {
            tmp[i][j] = grid[i][j];
        }
    }
    for (int i = 0; i < wsize->ws_row; i++) {
        for (int j = 0; j < wsize->ws_col; j++) {
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
    struct winsize wsize;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &wsize);

    if (wsize.ws_col == 0 || wsize.ws_row == 0) {
        wsize.ws_col = 30;
        wsize.ws_row = 30;
    } else {
        wsize.ws_col -= 10;
        wsize.ws_row -= 10;
    }

    int **grid = create_grid(&wsize);
    setup_grid(grid, &wsize);

    int gen = 1;
    draw_grid(grid, &wsize, gen);

    while (1) {
        ++gen;
        simulate(grid, &wsize);
        draw_grid(grid, &wsize, gen);
        usleep(50000);
    }
    return 0;
}
