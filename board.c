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

int COLORS[] = {
  0xff0000,
  0xff00ff,
  0xffff00,
  0x00ffff,
  0x0000ff,
  0xcccccc,
  0x00ff00
};

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
  int color;
} Piece;

void renderPiece(SDL_Renderer* renderer, Piece* piece){
  int y, x; 
  for(y=0; y<4; y++){
    for(x=0; x<4; x++){
      char* piece_array = piece->piece_array;
      if(piece_array[y*4+x]){
        int color = piece->color;
        int r = color >> 16 & 0xff;
        int g = color >> 8 & 0xff;
        int b = color & 0xff;
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_Rect rect = { (piece->x + x) * TILE_SIZE, (piece->y + y) * TILE_SIZE, TILE_SIZE - 1, TILE_SIZE - 1 };
        SDL_RenderFillRect(renderer, &rect);
      }
    }
  }
}

void resetPiece(Piece* piece){
  int pieceIndex = rand() % (sizeof(PIECES) / sizeof(char*));
  piece->x = BOARD_WIDTH/2 - 1;
  piece->y = 0;
  piece->color = COLORS[pieceIndex];
  memcpy(piece->piece_array, PIECES[pieceIndex], sizeof(PIECE_J));
}

int doesPieceCollideWithBoard(Piece* piece, int* board, int offsetX, int offsetY){
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

int pieceRightBounds(Piece* piece){
  //find piece's right boundary
  int x, y;
  int rightBounds = 0;
  for(y=0; y<4; y++){
    for(x=0; x<4; x++){
      int val = piece->piece_array[y*4 + x];
      if(val && x > rightBounds){
        rightBounds = x;
      }
    }
  }

  return rightBounds;
}

int pieceLeftBounds(Piece *piece){
  //find piece's left boundary
  int x, y;
  int leftBounds = INT_MAX;
  for(y=0; y<4; y++){
    for(x=0; x<4; x++){
      int val = piece->piece_array[y*4 + x];
      if(val && x < leftBounds){
        leftBounds = x;
      }
    }
  }

  return leftBounds;
}

void rotatePieceRight(Piece* piece, int* board){
  int x, y;
  Piece tempPiece;
  tempPiece.x = piece->x;
  tempPiece.y = piece->y;
  memcpy(tempPiece.piece_array, piece->piece_array, 4*4*sizeof(char));

  for(y=0; y<4; y++){
    for(x=0; x<4; x++){
      tempPiece.piece_array[x*4 + (3-y)] = piece->piece_array[y*4+x];
    }
  }

  int leftBounds = pieceLeftBounds(&tempPiece) + tempPiece.x;
  int rightBounds = pieceRightBounds(&tempPiece) + tempPiece.x;

  if(!doesPieceCollideWithBoard(&tempPiece, board, 0, 0) && leftBounds >= 0 && rightBounds < BOARD_WIDTH - 1){
    memcpy(piece->piece_array, tempPiece.piece_array, 4*4*sizeof(char));
  }
}

void movePieceRight(Piece* piece, int* board){
  int x, y;
  int rightBounds = pieceRightBounds(piece);

  //move if its a valid movement
  if(rightBounds + piece->x < BOARD_WIDTH - 1 && !doesPieceCollideWithBoard(piece, board , 1, 0)){
    piece->x += 1;
  }
}

void movePieceLeft(Piece* piece, int* board){
  int x, y;
  int leftBounds = pieceLeftBounds(piece);

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

void mergePieceIntoBoard(Piece* piece, int* board){
  int x, y;

  for(y=0; y<4; y++){
    for(x=0; x<4; x++){
      if(piece->piece_array[y*4 + x]){
        int boardX = piece->x + x;
        int boardY = piece->y + y;
        if(boardX < BOARD_WIDTH && boardY < BOARD_HEIGHT){
          board[boardY*BOARD_WIDTH + boardX] = piece->color;
        }
      }
    }
  }
}

void renderBoard(SDL_Renderer* renderer, int* board){
  int x, y;
  for(y=0; y<BOARD_HEIGHT; y++){
    for(x=0; x<BOARD_WIDTH; x++){
      int block = board[y*BOARD_WIDTH + x];
      if(block){
        int r = block >> 16 & 0xff;
        int g = block >>  8 & 0xff;
        int b = block & 0xff;
        SDL_Rect rect = { x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE - 1, TILE_SIZE - 1 };
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_RenderFillRect(renderer, &rect);
      }
    }
  }
}

int tick(Piece* currentPiece, int* board){
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
      printf("%d", board[y*BOARD_WIDTH + x] ? 1 : 0);
    }
    printf("\n");
  }
}

void killLines(int *board){
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
        memcpy(board + (y1*BOARD_WIDTH + x), board + ((y1-1)*BOARD_WIDTH + x), BOARD_WIDTH*sizeof(int));
      }
      memset(board, 0, BOARD_WIDTH*sizeof(int));
      // Do this to check the line again in case there's a match
      y--;
    }
  }
}
