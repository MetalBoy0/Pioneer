#include "evaluate.h"
#include "../Representation/board.h"
#include "../MoveGeneration/movegen.h"



float pieceTableValue(const int table[], int square, bool isWhite)
{
    return table[(isWhite ? indexToRank(square) * 8 + indexToFile(square) : square)];
}

float getTablePieceValues(pieceList pieces, bool isWhite, const int table[])
{
    float score = 0;
    for (int i = 0; i < pieces.count; i++)
    {
        score += pieceTableValue(table, pieces.pieces[i], isWhite);
    }
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
    score += board->score;
    return score * (board->isWhite*2-1) ;
}