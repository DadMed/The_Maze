#include "../inc/maze.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int TEX_WIDTH = 64;
const int TEX_HEIGHT = 64;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* wallTextures[4]; // Assuming 4 different wall textures
SDL_Texture* groundTexture = NULL;
SDL_Texture* skyTexture = NULL;

// Example map (24x24)
int worldMap[MAZE_MAP_WIDTH][MAZE_MAP_HEIGHT] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

// Player's position
double posX = 22, posY = 12;
// Direction vector
double dirX = -1, dirY = 0;
// Camera plane
double planeX = 0, planeY = 0.66;

const double moveSpeed = 0.05;
const double rotSpeed = 0.03;

void initialize_SDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }

    window = SDL_CreateWindow("Maze Game",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN);
    if (window == NULL) {
        fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        fprintf(stderr, "Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }
}

void loadTextures() {
    wallTextures[0] = IMG_LoadTexture(renderer, "textures/brick_wall.jpg");
    wallTextures[1] = IMG_LoadTexture(renderer, "textures/brick_floor.jpg");
    wallTextures[2] = IMG_LoadTexture(renderer, "textures/wooden_wall.jpg");
    wallTextures[3] = IMG_LoadTexture(renderer, "textures/dbz.jpg");
    groundTexture = IMG_LoadTexture(renderer, "textures/grass.jpg");
    skyTexture = IMG_LoadTexture(renderer, "textures/Sky.jpg");

    if (!groundTexture || !skyTexture) {
        fprintf(stderr, "Failed to load ground or sky texture! SDL_image Error: %s\n", IMG_GetError());
        exit(1);
    }

    for (int i = 0; i < 4; i++) {
        if (wallTextures[i] == NULL) {
            fprintf(stderr, "Failed to load texture %d! SDL_image Error: %s\n", i, IMG_GetError());
            exit(1);
        }
    }
}

void close_SDL() {
    for (int i = 0; i < 4; i++) {
        SDL_DestroyTexture(wallTextures[i]);
    }
    SDL_DestroyTexture(groundTexture);
    SDL_DestroyTexture(skyTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void drawWalls() {
    SDL_Rect groundRect = { 0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2 };
    SDL_Rect skyRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT / 2 };

    SDL_RenderCopy(renderer, groundTexture, NULL, &groundRect);
    SDL_RenderCopy(renderer, skyTexture, NULL, &skyRect);

    for (int x = 0; x < SCREEN_WIDTH; x++) {
        double cameraX = 2 * x / (double)SCREEN_WIDTH - 1;
        double rayDirX = dirX + planeX * cameraX;
        double rayDirY = dirY + planeY * cameraX;

        int mapX = (int)posX;
        int mapY = (int)posY;

        double sideDistX;
        double sideDistY;

        double deltaDistX = (rayDirX == 0) ? 1e30 : fabs(1 / rayDirX);
        double deltaDistY = (rayDirY == 0) ? 1e30 : fabs(1 / rayDirY);
        double perpWallDist;

        int stepX;
        int stepY;

        int hit = 0;
        int side;

        if (rayDirX < 0) {
            stepX = -1;
            sideDistX = (posX - mapX) * deltaDistX;
        } else {
            stepX = 1;
            sideDistX = (mapX + 1.0 - posX) * deltaDistX;
        }
        if (rayDirY < 0) {
            stepY = -1;
            sideDistY = (posY - mapY) * deltaDistY;
        } else {
            stepY = 1;
            sideDistY = (mapY + 1.0 - posY) * deltaDistY;
        }

        while (hit == 0) {
            if (sideDistX < sideDistY) {
                sideDistX += deltaDistX;
                mapX += stepX;
                side = 0;
            } else {
                sideDistY += deltaDistY;
                mapY += stepY;
                side = 1;
            }
            if (worldMap[mapX][mapY] > 0) hit = 1;
        }

        if (side == 0) perpWallDist = (mapX - posX + (1 - stepX) / 2) / rayDirX;
        else perpWallDist = (mapY - posY + (1 - stepY) / 2) / rayDirY;

        int lineHeight = (int)(SCREEN_HEIGHT / perpWallDist);

        int drawStart = -lineHeight / 2 + SCREEN_HEIGHT / 2;
        if (drawStart < 0) drawStart = 0;
        int drawEnd = lineHeight / 2 + SCREEN_HEIGHT / 2;
        if (drawEnd >= SCREEN_HEIGHT) drawEnd = SCREEN_HEIGHT - 1;

        int texNum = worldMap[mapX][mapY] - 1;

        double wallX;
        if (side == 0) wallX = posY + perpWallDist * rayDirY;
        else wallX = posX + perpWallDist * rayDirX;
        wallX -= floor((wallX));

        int texX = (int)(wallX * (double)TEX_WIDTH);
        if (side == 0 && rayDirX > 0) texX = TEX_WIDTH - texX - 1;
        if (side == 1 && rayDirY < 0) texX = TEX_WIDTH - texX - 1;

        for (int y = drawStart; y < drawEnd; y++) {
            int d = y * 256 - SCREEN_HEIGHT * 128 + lineHeight * 128;
            int texY = ((d * TEX_HEIGHT) / lineHeight) / 256;
            SDL_Rect srcRect = { texX, texY, 1, 1 };
            SDL_Rect dstRect = { x, y, 1, 1 };
            SDL_RenderCopy(renderer, wallTextures[texNum], &srcRect, &dstRect);
        }
    }
}

int main(int argc, char* args[]) {
    (void)argc;
    (void)args;
    initialize_SDL();
    loadTextures();

    int quit = 0;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            } else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_UP:
                        if (worldMap[(int)(posX + dirX * moveSpeed)][(int)posY] == 0) posX += dirX * moveSpeed;
                        if (worldMap[(int)posX][(int)(posY + dirY * moveSpeed)] == 0) posY += dirY * moveSpeed;
                        break;
                    case SDLK_DOWN:
                        if (worldMap[(int)(posX - dirX * moveSpeed)][(int)posY] == 0) posX -= dirX * moveSpeed;
                        if (worldMap[(int)posX][(int)(posY - dirY * moveSpeed)] == 0) posY -= dirY * moveSpeed;
                        break;
                    case SDLK_RIGHT:
                        {
                            double oldDirX = dirX;
                            dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
                            dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
                            double oldPlaneX = planeX;
                            planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
                            planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
                        }
                        break;
                    case SDLK_LEFT:
                        {
                            double oldDirX = dirX;
                            dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
                            dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);
                            double oldPlaneX = planeX;
                            planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
                            planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
                        }
                        break;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        drawWalls();

        SDL_RenderPresent(renderer);
    }

    close_SDL();
    return 0;
}
