#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "board.c"

void doTick(Piece* curPiece, int* board){
  int destroyed = tick(curPiece, board);
  if(destroyed){
    resetPiece(curPiece);
  }
  killLines(board);
}

int main(int argc, char** args){
  SDL_Window *window;
  SDL_Renderer *renderer;
  int running = 1;

  if(SDL_Init(SDL_INIT_VIDEO) != 0){
    printf("Unable to initialize SDL: %s\n", SDL_GetError());
    return -1;
  }

  window = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, BOARD_WIDTH * TILE_SIZE, BOARD_HEIGHT * TILE_SIZE, 0);
  if(window == NULL){
    printf("Unable to create window: %s\n", SDL_GetError());
    return -1;
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if(renderer == NULL){
    printf("Unable to create renderer: %s\n", SDL_GetError());
  }
  srand(time(NULL));

  Piece piece;
  resetPiece(&piece);

  int board[BOARD_HEIGHT*BOARD_WIDTH] = {0};

  uint64_t now = SDL_GetPerformanceCounter();
  uint64_t last = 0;
  double deltaTime = 0;
   
  double tickTimer = 0;

  while(running){
    last = now;
    now = SDL_GetPerformanceCounter();
    deltaTime = (double)((now - last)*1000 / (double)SDL_GetPerformanceFrequency());
    
    SDL_Event e;
    while(SDL_PollEvent(&e)){
      switch(e.type){
        case SDL_QUIT:
          running = 0;
          break;
        case SDL_KEYDOWN:
          if(e.key.keysym.sym == SDLK_LEFT){
            movePieceLeft(&piece, board);
          }else if(e.key.keysym.sym == SDLK_RIGHT){
            movePieceRight(&piece, board);
          }else if(e.key.keysym.sym == SDLK_r){
            rotatePieceRight(&piece, board);
          }else if(e.key.keysym.sym == SDLK_DOWN){
           doTick(&piece, board); 
           tickTimer = 0;
          }
          break;
      }
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if(tickTimer >= 1000){
      doTick(&piece, board);
      tickTimer = tickTimer - 1000;
    }

    if(&piece){
      renderPiece(renderer, &piece);
    }

    renderBoard(renderer, board);

    tickTimer += deltaTime;
    SDL_RenderPresent(renderer);
  }  

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  
  return 0;
}
