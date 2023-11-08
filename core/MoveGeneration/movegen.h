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

extern MoveList *generateMoves(Board *board, MoveList *moveList);

extern MoveList *generateLegalCastles(Board *board, MoveList *moveList);

extern MoveList *generateLegalCaptures(Board *board, MoveList *moveList);

extern MoveList *generateLegalPawnCaptures(Board *board, MoveList *moveList);
extern MoveList *generateLegalKnightCaptures(Board *board, MoveList *moveList);
extern MoveList *generateLegalBishopCaptures(Board *board, MoveList *moveList);
extern MoveList *generateLegalRookCaptures(Board *board, MoveList *moveList);
extern MoveList *generateLegalQueenCaptures(Board *board, MoveList *moveList);
extern MoveList *generateLegalKingCaptures(Board *board, MoveList *moveList);

extern MoveList *generateLegalNonCaptures(Board *board, MoveList *moveList);

extern MoveList *generateLegalPawnNonCaptures(Board *board, MoveList *moveList);
extern MoveList *generateLegalKnightNonCaptures(Board *board, MoveList *moveList);
extern MoveList *generateLegalBishopNonCaptures(Board *board, MoveList *moveList);
extern MoveList *generateLegalRookNonCaptures(Board *board, MoveList *moveList);
extern MoveList *generateLegalQueenNonCaptures(Board *board, MoveList *moveList);
extern MoveList *generateLegalKingNonCaptures(Board *board, MoveList *moveList);

#endif