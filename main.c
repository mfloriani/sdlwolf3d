#include "constants.h"
#include <stdio.h>
#include <SDL2/SDL.h>

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

int isGameRunning = FALSE;
float ticksLastFrame;
float playerX, playerY;

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
  playerX = 0;
  playerY = 0;
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
      break;
    default:
      break;
    }
  }
}

void update()
{
  while (!SDL_TICKS_PASSED(SDL_GetTicks(), ticksLastFrame + FRAME_TIME_LENGTH));

  float deltatime = (SDL_GetTicks() - ticksLastFrame) / 1000.0f;
  ticksLastFrame = SDL_GetTicks();

  playerX += 50 * deltatime;
  playerY += 50 * deltatime;
}

void render()
{
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
  SDL_Rect rect = {playerX, playerY, 20, 20};
  SDL_RenderFillRect(renderer, &rect);

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