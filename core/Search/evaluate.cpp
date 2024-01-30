#include "evaluate.h"
#include "../Representation/board.h"
#include "../MoveGeneration/movegen.h"

const int pawnTable[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
    5, 5, 10, 25, 25, 10, 5, 5,
    0, 0, 0, 20, 20, 0, 0, 0,
    5, -5, -10, 0, 0, -10, -5, 5,
    5, 10, 10, -20, -20, 10, 10, 5,
    0, 0, 0, 0, 0, 0, 0, 0};
const int rooks[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    5, 10, 10, 10, 10, 10, 10, 5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    -5, 0, 0, 0, 0, 0, 0, -5,
    0, 0, 0, 5, 5, 0, 0, 0};
const int knights[64] = {
    -50, -40, -30, -30, -30, -30, -40, -50,
    -40, -20, 0, 0, 0, 0, -20, -40,
    -30, 0, 10, 15, 15, 10, 0, -30,
    -30, 5, 15, 20, 20, 15, 5, -30,
    -30, 0, 15, 20, 20, 15, 0, -30,
    -30, 5, 10, 15, 15, 10, 5, -30,
    -40, -20, 0, 5, 5, 0, -20, -40,
    -50, -40, -30, -30, -30, -30, -40, -50};
const int bishops[64] = {
    -20, -10, -10, -10, -10, -10, -10, -20,
    -10, 0, 0, 0, 0, 0, 0, -10,
    -10, 0, 5, 10, 10, 5, 0, -10,
    -10, 5, 5, 10, 10, 5, 5, -10,
    -10, 0, 10, 10, 10, 10, 0, -10,
    -10, 10, 10, 10, 10, 10, 10, -10,
    -10, 5, 0, 0, 0, 0, 5, -10,
    -20, -10, -10, -10, -10, -10, -10, -20};
const int queens[64] = {
    -20, -10, -10, -5, -5, -10, -10, -20,
    -10, 0, 0, 0, 0, 0, 0, -10,
    -10, 0, 5, 5, 5, 5, 0, -10,
    -5, 0, 5, 5, 5, 5, 0, -5,
    0, 0, 5, 5, 5, 5, 0, -5,
    -10, 5, 5, 5, 5, 5, 0, -10,
    -10, 0, 5, 0, 0, 0, 0, -10,
    -20, -10, -10, -5, -5, -10, -10, -20};
const int kings[64] = {
    -80, -70, -70, -70, -70, -70, -70, -80,
    -60, -60, -60, -60, -60, -60, -60, -60,
    -40, -50, -50, -60, -60, -50, -50, -40,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -20, -30, -30, -40, -40, -30, -30, -20,
    -10, -20, -20, -20, -20, -20, -20, -10,
    20, 20, -5, -5, -5, -5, 20, 20,
    20, 30, 10, 0, 0, 10, 30, 20};

float pieceTableValue(const int table[64], int square, bool isWhite)
{
    int index = square;
    if (!isWhite)
    {
        int rank = indexToRank(square);
        index = rank * 8 + indexToFile(square);
    }
    return table[index];
}

float getTablePieceValues(pieceList pieces, bool isWhite, const int table[64])
{
    float score = 0;
    for (int i = 0; i < pieces.count; i++)
    {
        score += pieceTableValue(table, pieces.pieces[i], isWhite);
    }
    return score;
}

float TableValues(Board *board, bool isWhite)
{
    int colorIndex = isWhite ? 0 : 1;
    float score = 0;
    score += getTablePieceValues(board->pieces[Pieces::Pawn][colorIndex], isWhite, pawnTable);
    score += getTablePieceValues(board->pieces[Pieces::Knight][colorIndex], isWhite, knights);
    score += getTablePieceValues(board->pieces[Pieces::Bishop][colorIndex], isWhite, bishops);
    score += getTablePieceValues(board->pieces[Pieces::Rook][colorIndex], isWhite, rooks);
    score += getTablePieceValues(board->pieces[Pieces::Queen][colorIndex], isWhite, queens);
    score += getTablePieceValues(board->pieces[Pieces::King][colorIndex], isWhite, kings);
    return score;
}

float evaluate(Board *board)
{
    float score = 0;

    // Add up piece values
    score += pieceValues[Pieces::Pawn] * board->pieces[Pieces::Pawn][0].count;
    score -= pieceValues[Pieces::Pawn] * board->pieces[Pieces::Pawn][1].count;
    score += pieceValues[Pieces::Knight] * board->pieces[Pieces::Knight][0].count;
    score -= pieceValues[Pieces::Knight] * board->pieces[Pieces::Knight][1].count;
    score += pieceValues[Pieces::Bishop] * board->pieces[Pieces::Bishop][0].count;
    score -= pieceValues[Pieces::Bishop] * board->pieces[Pieces::Bishop][1].count;
    score += pieceValues[Pieces::Rook] * board->pieces[Pieces::Rook][0].count;
    score -= pieceValues[Pieces::Rook] * board->pieces[Pieces::Rook][1].count;
    score += pieceValues[Pieces::Queen] * board->pieces[Pieces::Queen][0].count;
    score -= pieceValues[Pieces::Queen] * board->pieces[Pieces::Queen][1].count;
    score += pieceValues[Pieces::King] * board->pieces[Pieces::King][0].count;
    score -= pieceValues[Pieces::King] * board->pieces[Pieces::King][1].count;


    // Piece Tables
    score += TableValues(board, true);
    score -= TableValues(board, false);
    return score * (board->isWhite ? 1 : -1);
}