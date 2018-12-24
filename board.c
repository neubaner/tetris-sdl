#include <SDL2/SDL.h>
#include <limits.h>

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20
#define TILE_SIZE 32 

char PIECE_I[] = {
  0, 0, 1, 0,
  0, 0, 1, 0,
  0, 0, 1, 0,
  0, 0, 1, 0
};

char PIECE_J[] = {
  0, 1, 1, 0,
  0, 0, 1, 0,
  0, 0, 1, 0,
  0, 0, 0, 0
};

char PIECE_L[] = {
  0, 1, 1, 0,
  0, 1, 0, 0,
  0, 1, 0, 0,
  0, 0, 0, 0
};

char PIECE_O[] = {
  0, 1, 1, 0,
  0, 1, 1, 0,
  0, 0, 0, 0,
  0, 0, 0, 0
};

char PIECE_S[] = {
  0, 1, 1, 0,
  1, 1, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0
};

char PIECE_T[] = {
  1, 1, 1, 0,
  0, 1, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0
};

char PIECE_Z[] = {
  0, 1, 1, 0,
  0, 0, 1, 1,
  0, 0, 0, 0,
  0, 0, 0, 0
};

char *PIECES[] = { PIECE_I, PIECE_J, PIECE_L, PIECE_O, PIECE_S, PIECE_T, PIECE_Z }; 

enum PieceType {  
 PIECE_TYPE_I,
 PIECE_TYPE_J,
 PIECE_TYPE_L,
 PIECE_TYPE_O,
 PIECE_TYPE_S,
 PIECE_TYPE_T,
 PIECE_TYPE_Z,
};

typedef struct {
  int x;
  int y;
  char piece_array[4*4];
} Piece;

void renderPiece(SDL_Renderer* renderer, Piece* piece){
  int y, x; 
  for(y=0; y<4; y++){
    for(x=0; x<4; x++){
      char* piece_array = piece->piece_array;
      if(piece_array[y*4+x]){
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_Rect rect = { (piece->x + x) * TILE_SIZE, (piece->y + y) * TILE_SIZE, TILE_SIZE, TILE_SIZE };
        SDL_RenderDrawRect(renderer, &rect);
      }
    }
  }
}

void resetPiece(Piece* piece){
  int pieceIndex = rand() % (sizeof(PIECES) / sizeof(char*));
  piece->x = BOARD_WIDTH/2 - 1;
  piece->y = 0;
  memcpy(piece->piece_array, PIECES[pieceIndex], sizeof(PIECE_J));
}

void rotatePieceRight(Piece* piece){
  int x, y;
  char tempPiece[4*4];
  memcpy(tempPiece, piece->piece_array, 4*4*sizeof(char));

  for(y=0; y<4; y++){
    for(x=0; x<4; x++){
      tempPiece[x*4 + (3-y)] = piece->piece_array[y*4+x];
    }
  }

  memcpy(piece->piece_array, tempPiece, 4*4*sizeof(char));
}

int doesPieceCollideWithBoard(Piece* piece, char* board, int offsetX, int offsetY){
  int x, y;
  for(y=0; y<4; y++){
    for(x=0; x<4; x++){
      int boardX = piece->x + x + offsetX;
      int boardY = piece->y + y + offsetY;
      
      if(boardX < BOARD_WIDTH && boardY < BOARD_HEIGHT &&
         piece->piece_array[y*4 + x] && board[boardY*BOARD_WIDTH + boardX]){
        return 1;
      }
    }
  }

  return 0;
}

void movePieceRight(Piece* piece, char* board){
  int x, y;

  //find piece's right boundary
  int rightBounds = 0;
  for(y=0; y<4; y++){
    for(x=0; x<4; x++){
      int val = piece->piece_array[y*4 + x];
      if(val && x > rightBounds){
        rightBounds = x;
      }
    }
  }

  //move if its a valid movement
  if(rightBounds + piece->x < BOARD_WIDTH - 1 && !doesPieceCollideWithBoard(piece, board , 1, 0)){
    piece->x += 1;
  }
}

void movePieceLeft(Piece* piece, char* board){
  int x, y;

  //find piece's left boundary
  int leftBounds = INT_MAX;
  for(y=0; y<4; y++){
    for(x=0; x<4; x++){
      int val = piece->piece_array[y*4 + x];
      if(val && x < leftBounds){
        leftBounds = x;
      }
    }
  }

  //move if its a valid movement
  if(leftBounds + piece->x > 0 && !doesPieceCollideWithBoard(piece, board, -1, 0)){
    piece->x -= 1;
  }
}

int findPieceGround(Piece* piece){
  int x, y;
  int ground=0;

  for(y=0; y<4; y++){
    for(x=0; x<4; x++){
      if(piece->piece_array[y*4 + x] && ground < y){
        ground = y;
      }
    }
  }

  return ground;
}

void mergePieceIntoBoard(Piece* piece, char* board){
  int x, y;

  for(y=0; y<4; y++){
    for(x=0; x<4; x++){
      if(piece->piece_array[y*4 + x]){
        int boardX = piece->x + x;
        int boardY = piece->y + y;
        if(boardX < BOARD_WIDTH && boardY < BOARD_HEIGHT){
          board[boardY*BOARD_WIDTH + boardX] = 1;
        }
      }
    }
  }
}

void renderBoard(SDL_Renderer* renderer, char* board){
  int x, y;
  for(y=0; y<BOARD_HEIGHT; y++){
    for(x=0; x<BOARD_WIDTH; x++){
      if(board[y*BOARD_WIDTH + x]){
        SDL_Rect rect = { x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE };
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &rect);
      }
    }
  }
}

int tick(Piece* currentPiece, char* board){
  int x, y;
  int destroyed = 0;
  if(currentPiece != NULL){

    int pieceGround = findPieceGround(currentPiece);
    int pieceLB = currentPiece->y + pieceGround;

    if(pieceLB >= BOARD_HEIGHT - 1 || doesPieceCollideWithBoard(currentPiece, board, 0, 1)){
      mergePieceIntoBoard(currentPiece, board); 
      destroyed = 1;
    }

    currentPiece->y += 1;
  }

  return destroyed;
}

void printBoard(char *board){
  int x, y;
  for(y=0; y<BOARD_HEIGHT; y++){
    for(x=0; x<BOARD_WIDTH; x++){
      printf("%d", board[y*BOARD_WIDTH + x]);
    }
    printf("\n");
  }
}

void killLines(char *board){
  int x, y, y1;
  for(y=0; y<BOARD_HEIGHT; y++){
    int dead = 1;
    for(x=0; x<BOARD_WIDTH; x++){
      if(board[y*BOARD_WIDTH + x] == 0){
        dead = 0;
        break;
      }
    }
    if(dead){
      for(y1=y-1; y1>0; y1--){
        memcpy(board + (y1*BOARD_WIDTH + x), board + ((y1-1)*BOARD_WIDTH + x), BOARD_WIDTH*sizeof(char));
      }
      memset(board, 0, BOARD_WIDTH*sizeof(char));
      // Do this to check the line again in case there's a match
      y--;
    }
  }
}
