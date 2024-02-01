#include "moveOrder.h"
#include <stdlib.h>

using namespace std;
struct MoveScore
{
    Move move;
    int score;
};

void sortMoves(MoveList *moveList, Move prevMove, Board *board, bool onlyCaptures)
{
    MoveScore moveScores[256];
    int moveCount = moveList->count;
    for (int i = 0; i < moveCount; i++)
    {
        moveScores[i].move = moveList->moves[i];
        moveScores[i].score = 0;
        int from = getFrom(moveScores[i].move);
        if (moveScores[i].move == prevMove)
        {
            moveScores[i].score += 1000000;
        }
        if (board->isAttacked(from, board->isWhite ? Pieces::Black : Pieces::White) && !Pieces::isPawn(board->board[from]))
        {
            moveScores[i].score += 200000;
        }
        if (isCapture(moveScores[i].move))
        {
            moveScores[i].score += (100000 + (Pieces::getType(getCapturedPiece(moveScores[i].move)) - Pieces::getType(board->board[from])));
        }
        if (isPromotion(moveScores[i].move))
        {
            moveScores[i].score += 200000;
        }
        if (board->isEnPassant(moveScores[i].move))
        {
            moveScores[i].score += 100000;
        }
        if (isCastle(moveScores[i].move))
        {
            moveScores[i].score += 100000;
        }
        if (Pieces::getType(board->board[from]) == Pieces::King)
        {
            moveScores[i].score -= 10;
        }
    }
    for (int i = 0; i < moveCount; i++)
    {
        for (int j = i + 1; j < moveCount; j++)
        {
            if (moveScores[j].score > moveScores[i].score)
            {
                MoveScore temp = moveScores[i];
                moveScores[i] = moveScores[j];
                moveScores[j] = temp;
                moveList->moves[i] = moveScores[i].move;
                moveList->moves[j] = moveScores[j].move;
            }
        }
    }
}