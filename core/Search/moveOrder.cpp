#include "moveOrder.h"

struct MoveScore
{
    Move move;
    int score;
};

void quickSort(MoveScore *moveScores, int left, int right)
{
    int i = left, j = right;
    MoveScore tmp;
    int pivot = moveScores[(left + right) / 2].score;

    /* partition */
    while (i <= j)
    {
        while (moveScores[i].score > pivot)
            i++;
        while (moveScores[j].score < pivot)
            j--;
        if (i <= j)
        {
            tmp = moveScores[i];
            moveScores[i] = moveScores[j];
            moveScores[j] = tmp;
            i++;
            j--;
        }
    };

    /* recursion */
    if (left < j)
        quickSort(moveScores, left, j);
    if (i < right)
        quickSort(moveScores, i, right);
}

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
    quickSort(moveScores, 0, moveCount - 1);
    for (int i = 0; i < moveCount; i++)
    {
        moveList->moves[i] = moveScores[i].move;
    }
}