#include "Arduino.h"
#include "GameOfLife.h"

GameOfLife	life;

GameOfLife::GameOfLife(){};

// Handler for new game button
void GameOfLife::newGame() {
  //uView.clear(PAGE, 0);
  randSeed++;
  genGrid(randSeed);
}

// Handler for restart button
void GameOfLife::replayGame() {
 // uView.clear(PAGE, 0);
  genGrid(randSeed);
}

// Generate a new game grid.
// Places a random number of random characters from the current font
// at random positions on the screen.
void GameOfLife::genGrid(long seed) {

  randomSeed(seed);

  for(int row=0; row < LIFEHEIGHT/8; row++){
    for(int col=0; col < LIFEWIDTH; col++){
      if (random(4) == 0) {
        grid[row][col] = random(127);
      } else {
        grid[row][col] = 0;
      }
    }
  }
}
//------------------------------------------------------------
// The Life engine
//
// Updates the provided grid with the next generation.
//
// Only the torus type finite grid is implemented. (Left edge joins to the
// right edge and top edge joins to the bottom edge.)
//
// The grid is mapped as horizontal rows of bytes where each byte is a
// vertical line of cells with the least significant bit being the top cell.

// LIFEWIDTH and LIFEHEIGHT must have been previously defined.
// LIFEWIDTH is the width of the grid.
// LIFEHEIGHT is the height of the grid and must be a multiple of 8.


// number of bits in values from 0 to 7
static const unsigned char bitCount[] = { 0, 1, 1, 2, 1, 2, 2, 3 };

void GameOfLife::lifeIterate(uint8_t grid[][LIFEWIDTH]) {
  Serial.println("Triggering LifeIterate"  + String(millis()));
  uint8_t cur[LIFELINES][LIFEWIDTH]; // working copy of the current grid

  unsigned char row, col; // current row & column numbers
  unsigned char rowA, rowB, colR; // row above, row below, column to the right
  unsigned int left, centre, right; // packed vertical cell groups

  memcpy(cur, grid, sizeof cur);

  rowA = LIFEHIGHROW;
  rowB = 1;
  for (row = 0; row < LIFELINES ; row++) {
    left = (((unsigned int) (cur[rowA][LIFEHIGHCOL])) >> 7) |
           (((unsigned int) cur[row][LIFEHIGHCOL]) << 1) |
           (((unsigned int) cur[rowB][LIFEHIGHCOL]) << 9);

    centre = (((unsigned int) (cur[rowA][0])) >> 7) |
             (((unsigned int) cur[row][0]) << 1) |
             (((unsigned int) cur[rowB][0]) << 9);

    colR = 1;
    for (col=0; col < LIFEWIDTH; col++) {
      right = (((unsigned int) (cur[rowA][colR])) >> 7) |
              (((unsigned int) cur[row][colR]) << 1) |
              (((unsigned int) cur[rowB][colR]) << 9);

      grid[row][col] = lifeByte(left, centre, right);

      left = centre;
      centre = right;

      colR = (colR < LIFEHIGHCOL) ? colR + 1 : 0;
    }
    rowA = (rowA < LIFEHIGHROW) ? rowA + 1 : 0;
    rowB = (rowB < LIFEHIGHROW) ? rowB + 1 : 0;
  }
    Serial.println("LifeIterate complete " + String(millis())  );

}

// Calculate the next generation for 8 vertical cells (one byte of the
// array) that have been packet along with their neighbours into ints.
uint8_t GameOfLife::lifeByte(unsigned int left, unsigned int centre, unsigned int right) {
  unsigned char count;
  uint8_t newByte = 0;

  for (unsigned char i=0; i<8; i++) {
    count = bitCount[left & 7] + bitCount[centre & 7] + bitCount[right & 7];

    if ((count == 3) || ((count == 4) && (centre & 2))) {
      newByte |= (1 << i);
    }
    left >>= 1;
    centre >>= 1;
    right >>= 1;
  }
  return newByte;
}
