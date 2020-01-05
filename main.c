#include "constants.h"
#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>

const int map[MAP_NUM_ROWS][MAP_NUM_COLS] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

struct Player
{
  float x;
  float y;
  float width;
  float height;
  int turnDirection; // -1 left, 1 right
  int walkDirection; // -1 back, 1 front
  float rotationAngle;
  float walkSpeed;
  float turnSpeed;
} player;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

int isGameRunning = FALSE;
float ticksLastFrame;

void renderMap()
{
  for (int i = 0; i < MAP_NUM_ROWS; i++)
  {
    for (int j = 0; j < MAP_NUM_COLS; j++)
    {
      int tileX = j * TILE_SIZE;
      int tileY = i * TILE_SIZE;
      int tileColor = map[i][j] != 0 ? 255 : 0;
      SDL_SetRenderDrawColor(renderer, tileColor, tileColor, tileColor, 255);
      SDL_Rect mapTileRect = {
          tileX * MINIMAP_SCALE_FACTOR,
          tileY * MINIMAP_SCALE_FACTOR,
          TILE_SIZE * MINIMAP_SCALE_FACTOR,
          TILE_SIZE * MINIMAP_SCALE_FACTOR};
      SDL_RenderFillRect(renderer, &mapTileRect);
    }
  }
}

int hasWallAt(float x, float y)
{
  if(x < 0 || x > WINDOW_WIDTH || y < 0 || y > WINDOW_HEIGHT) return TRUE;
  
  int gridX = floor(x / TILE_SIZE);
  int gridY = floor(y / TILE_SIZE);
  int tile = map[gridY][gridX];
  return tile != 0;
}

void updatePlayer(float deltatime)
{
  player.rotationAngle += player.turnDirection * player.turnSpeed * deltatime;
  
  float moveStep = player.walkDirection * player.walkSpeed * deltatime;
  
  float newX = player.x + cos(player.rotationAngle) * moveStep;
  float newY = player.y + sin(player.rotationAngle) * moveStep;
  
  if(!hasWallAt(newX, newY)){
    player.x = newX;
    player.y = newY;
  }
}

void renderPlayer()
{
  SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
  SDL_Rect playerRect = {
      player.x * MINIMAP_SCALE_FACTOR,
      player.y * MINIMAP_SCALE_FACTOR,
      player.width * MINIMAP_SCALE_FACTOR,
      player.height * MINIMAP_SCALE_FACTOR};
  SDL_RenderFillRect(renderer, &playerRect);

  SDL_RenderDrawLine(
      renderer,
      player.x * MINIMAP_SCALE_FACTOR,
      player.y * MINIMAP_SCALE_FACTOR,
      player.x + cos(player.rotationAngle) * 40 * MINIMAP_SCALE_FACTOR,
      player.y + sin(player.rotationAngle) * 40 * MINIMAP_SCALE_FACTOR);
}

int init()
{
  if (SDL_Init(SDL_INIT_EVERYTHING))
  {
    fprintf(stderr, "Error initializing SDL.\n");
    return FALSE;
  }
  window = SDL_CreateWindow("Wolf3D", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_BORDERLESS);
  if (!window)
  {
    fprintf(stderr, "Error creating SDL window. \n");
    return FALSE;
  }
  renderer = SDL_CreateRenderer(window, -1, 0);
  if (!renderer)
  {
    fprintf(stderr, "Error creating SDL renderer.\n");
    return FALSE;
  }
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  return TRUE;
}

void setup()
{
  player.x = WINDOW_WIDTH / 2;
  player.y = WINDOW_HEIGHT / 2;
  player.width = 5;
  player.height = 5;
  player.turnDirection = 0;
  player.walkDirection = 0;
  player.rotationAngle = PI / 2;
  player.walkSpeed = 100;
  player.turnSpeed = 45 * (PI / 100);
}

void handleInput()
{
  SDL_Event event;
  while (SDL_PollEvent(&event))
  {
    switch (event.type)
    {
    case SDL_QUIT:
      isGameRunning = FALSE;
      break;
    case SDL_KEYDOWN:
      if (event.key.keysym.sym == SDLK_ESCAPE)
      {
        isGameRunning = FALSE;
      }
      if (event.key.keysym.sym == SDLK_UP)
      {
        player.walkDirection = 1;
      }
      if (event.key.keysym.sym == SDLK_DOWN)
      {
        player.walkDirection = -1;
      }
      if (event.key.keysym.sym == SDLK_RIGHT)
      {
        player.turnDirection = 1;
      }
      if (event.key.keysym.sym == SDLK_LEFT)
      {
        player.turnDirection = -1;
      }
      break;
    case SDL_KEYUP:
      if (event.key.keysym.sym == SDLK_UP)
      {
        player.walkDirection = 0;
      }
      if (event.key.keysym.sym == SDLK_DOWN)
      {
        player.walkDirection = 0;
      }
      if (event.key.keysym.sym == SDLK_RIGHT)
      {
        player.turnDirection = 0;
      }
      if (event.key.keysym.sym == SDLK_LEFT)
      {
        player.turnDirection = 0;
      }
      break;
    default:
      break;
    }
  }
}

void update()
{
  while (!SDL_TICKS_PASSED(SDL_GetTicks(), ticksLastFrame + FRAME_TIME_LENGTH))
    ;
  float deltatime = (SDL_GetTicks() - ticksLastFrame) / 1000.0f;
  ticksLastFrame = SDL_GetTicks();

  updatePlayer(deltatime);
}

void render()
{
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  renderMap();
  //renderRays();
  renderPlayer();

  SDL_RenderPresent(renderer);
}

void quit()
{
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

int main(int argc, char *args[])
{
  isGameRunning = init();

  setup();

  while (isGameRunning)
  {
    handleInput();
    update();
    render();
  }
  quit();
  return 0;
}