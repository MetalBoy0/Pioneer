//
// Created by Will on 8/20/2023.
//

#ifndef CHESSENGINE_AI_H
#define CHESSENGINE_AI_H
#include "main.h"

float minMax(Board* board, Move* thisMove, int depth, int ply, float alpha, float beta);
Move* findBestMove(Board* board);


#endif //CHESSENGINE_AI_H
