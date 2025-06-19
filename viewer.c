// CGA Viewer - A image viewer for my 3-bit color cga file format
// Copyright (C) 2025 Connor Thomson
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or 
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
// Compile: gcc viewer.c -o viewer `sdl2-config --cflags --libs`

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define MAX_LINES 1024
#define MAX_LINE_LENGTH 1024

SDL_Color palette[8] = {
    {0, 0, 0, 255},       // 1 - Black
    {255, 0, 0, 255},     // 2 - Red
    {0, 255, 0, 255},     // 3 - Green
    {0, 0, 255, 255},     // 4 - Blue
    {255, 255, 0, 255},   // 5 - Yellow
    {255, 0, 255, 255},   // 6 - Magenta
    {0, 255, 255, 255},   // 7 - Cyan
    {255, 255, 255, 255}  // 8 - White
};

int parse_file(const char* filename, char lines[MAX_LINES][MAX_LINE_LENGTH], int* width, int* height) {
    FILE* f = fopen(filename, "r");
    if (!f) return 0;

    int max_width = 0, line_count = 0;
    char buffer[MAX_LINE_LENGTH];
    while (fgets(buffer, sizeof(buffer), f)) {
        char* semi = strchr(buffer, ';');
        if (semi) {
            *semi = '\0';
            int len = strlen(buffer);
            if (len > max_width) max_width = len;
            strcpy(lines[line_count++], buffer);
        }
    }

    fclose(f);
    *width = max_width;
    *height = line_count;
    return 1;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Input file needed\n", argv[0]);
        return 1;
    }

    char lines[MAX_LINES][MAX_LINE_LENGTH];
    int img_width = 0, img_height = 0;
    if (!parse_file(argv[1], lines, &img_width, &img_height)) {
        fprintf(stderr, "Failed to load file.\n");
        return 1;
    }

    int zoom_x = WINDOW_WIDTH / img_width;
    int zoom_y = WINDOW_HEIGHT / img_height;
    int zoom = zoom_x < zoom_y ? zoom_x : zoom_y;

    int draw_width = img_width * zoom;
    int draw_height = img_height * zoom;

    int offset_x = (WINDOW_WIDTH - draw_width) / 2;
    int offset_y = (WINDOW_HEIGHT - draw_height) / 2;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* win = SDL_CreateWindow("Computer Graphics Array Viewer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
    SDL_RenderClear(ren);

    for (int y = 0; y < img_height; ++y) {
        for (int x = 0; x < img_width && lines[y][x] != '\0'; ++x) {
            int index = lines[y][x] - '1';
            if (index >= 0 && index < 8) {
                SDL_SetRenderDrawColor(ren,
                    palette[index].r,
                    palette[index].g,
                    palette[index].b,
                    255);
                SDL_Rect rect = {
                    offset_x + x * zoom,
                    offset_y + y * zoom,
                    zoom,
                    zoom
                };
                SDL_RenderFillRect(ren, &rect);
            }
        }
    }

    SDL_RenderPresent(ren);

    SDL_Event e;
    while (SDL_WaitEvent(&e)) {
        if (e.type == SDL_QUIT) break;
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
