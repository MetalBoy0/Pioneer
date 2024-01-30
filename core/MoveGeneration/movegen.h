#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "..\Representation\move.h"
#include "..\Representation\board.h"

struct MoveList
{
    Move moves[256];
    int count;
};


extern void appendMove(MoveList *moveList, Move move);
extern void generateMoves(Board *board, MoveList *moveList, bool onlyCaptures = false);

#endif