#ifndef EVALUATE_H
#define EVALUATE_H

#include "..\Representation\board.h"

const int pieceValues[7] = {0, 100, 300, 300, 500, 900, 100000 };

extern float evaluate(Board *board);


#endif