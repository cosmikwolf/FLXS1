#ifndef _GameOfLife_h_
#define _GameOfLife_h_
// https://github.com/MLXXXp/MicroViewLife/blob/master/MicroViewLife.ino
// http://hujackus.altervista.org/conwaymatrix/

#define LIFEWIDTH 16
#define LIFEHEIGHT 96 // must be a multiple of 8

#define LIFELINES (LIFEHEIGHT / 8)
#define LIFEHIGHROW (LIFELINES - 1)
#define LIFEHIGHCOL (LIFEWIDTH - 1)

class GameOfLife
{

public:
	GameOfLife();
	void newGame();
	void replayGame();
	void genGrid(long seed);
	void lifeIterate(uint8_t grid[][LIFEWIDTH]);
	uint8_t lifeByte(unsigned int left, unsigned int centre, unsigned int right);

	uint8_t grid[LIFEHEIGHT/8][LIFEWIDTH];

	long randSeed = 1245;
private:

};


#endif