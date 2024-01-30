#ifndef MOVEORDER_H
#define MOVEORDER_H

#include "..\Representation\board.h"
#include "..\MoveGeneration\movegen.h"

extern void sortMoves(MoveList *moves, Move prevMove, Board *board, bool onlyCaptures = false);

#endif