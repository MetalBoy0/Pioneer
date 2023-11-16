#include "movegen.h"
#include "../Representation/piece.h"
int captures = 0;
int checks = 0;

bool distIsMoreThanOne(int from, int to)
{
    return (abs(indexToRank(from) - indexToRank(to)) > 1) || (abs(indexToFile(from) - indexToFile(to)) > 1);
}

void appendMove(MoveList *movelist, Move move)
{
    movelist->moves[movelist->count++] = move;
}

Bitboard generateSlidingMovesBB(Board *b, int from, Direction moves[], bool isQueen)
{
    Bitboard bb = 0LL;
    int t = 4 * (isQueen + 1);
    for (int i = 0; i < t; i++)
    {
        int offset = moves[i];
        int to = from + offset;
        while (indexToRank(to) >= 0 && indexToRank(to) <= 7 && indexToFile(to) >= 0 && indexToFile(to) <= 7)
        {
            if (b->colorBB[b->sideToMove] & (1 << to))
            {
                break;
            }
            if (b->colorBB[b->otherSide] & (1 << to))
            {
                setBit(&bb, to);
                break;
            }
            setBit(&bb, to);
            to += offset;
        }
        return bb;
    }
}

// Bitboard of all pawn capturesS
Bitboard pawnCaptureBitboards[Pieces::Black][64];

// Bitboard of all pawn moves
// Does not include double pawn moves
Bitboard pawnMoveBitboards[Pieces::Black][64];

// Bitboard of all knight moves
Bitboard knightMoveBitboards[64];

// Bitboard of all king moves
// Does not check for wrapping around the board
Bitboard kingMoveBitboards[64];

int bishopMoveOffsets[] = {7, 9, -7, -9};
int rookMoveOffsets[] = {8, -8, 1, -1};
int queenMoveOffsets[] = {7, 9, -7, -9, 8, -8, 1, -1};

// Setup move bitboards
void init()
{
    for (int i = 0; i < 64; i++)
    {
        if (indexToRank(i) < 7 && indexToRank(i) > 0)
        {
            if (indexToFile(i) > 0)
            {
                pawnCaptureBitboards[Pieces::Black][i] |= (1 << (i + 7));
                pawnCaptureBitboards[Pieces::White][i] |= (1 << (i - 9));
            }
            if (indexToFile(i) < 7)
            {
                pawnCaptureBitboards[Pieces::Black][i] |= (1 << (i + 9));
                pawnCaptureBitboards[Pieces::White][i] |= (1 << (i - 7));
            }
            pawnMoveBitboards[Pieces::Black][i] = (1 << (i + 8));
            pawnMoveBitboards[Pieces::White][i] = (1 << (i - 8));
        }
        else
        {
            pawnCaptureBitboards[Pieces::Black][i] = 0;
            pawnCaptureBitboards[Pieces::White][i] = 0;
        }
        int knightMoves[] = {17, 15, 10, 6, -17, -15, -10, -6};
        for (int j = 0; j < 8; j++)
        {
            if (abs(indexToFile(i + knightMoves[i]) - indexToFile(i)) > 2)
            {
                continue;
            }
            knightMoveBitboards[i] |= (1 << (i + knightMoves[i]));
        }

        kingMoveBitboards[i] = (1 << (i + 9)) | (1 << (i + 8)) | (1 << (i + 7)) | (1 << (i + 1)) |
                               (1 << (i - 1)) | (1 << (i - 7)) | (1 << (i - 8)) | (1 << (i - 9));
    }
}

void generateMoves(Board *board, MoveList *moveList, bool capturesOnly)
{
    moveList->count = 0;
    generateLegalNonCaptures(board, moveList);
}

MoveList *generateLegalCaptures(Board *board, MoveList *moves)
{
    // Pawns
    Bitboard pawns = board->colorBB[board->sideToMove] && board->pieceBB[Pieces::Pawn];
    Bitboard enPassant = 0;
    if (board->enPassantSquare != -1)
    {
        enPassant = (1 << board->enPassantSquare);
    }
    while (pawns)
    {
        int pawn = popLSB(&pawns);
        Bitboard captures = pawnCaptureBitboards[board->sideToMove][pawn] & emptyBB;
        while (captures)
        {
            int capture = popLSB(&captures);
            if (indexToRank(capture) == 0 || indexToRank(capture) == 7)
            {
                appendMove(moves, board->getMove(pawn, capture, Pieces::Queen));
                appendMove(moves, board->getMove(pawn, capture, Pieces::Rook));
                appendMove(moves, board->getMove(pawn, capture, Pieces::Bishop));
                appendMove(moves, board->getMove(pawn, capture, Pieces::Knight));
            }
            else
            {
                appendMove(moves, board->getMove(pawn, capture));
            }
            if (capture == enPassant)
            {
                appendMove(&moves, board->getMove(pawn, capture));
            }
        }
    }

    // Knights
    Bitboard knights = board->colorBB[board->sideToMove] && board->pieceBB[Pieces::Knight];
    Bitboard enemyPieces = board->colorBB[board->otherSide];
    while (knights)
    {
        int knight = popLSB(&knights);
        Bitboard captures = knightMoveBitboards[knight] & enemyPieces;
        while (captures)
        {
            int capture = popLSB(&captures);
            appendMove(moves, board->getMove(knight, capture));
        }
    }

    generateLegalKingCaptures();
    return &moves;
}

void generateLegalSlidingMoves(Board *board, MoveList *moves, bool capturesOnly)
{
    Direction offsets[2][4] = {{NW, NE, SW, SE}, {N, W, E, S}, {NULLDIR, NULLDIR, NULLDIR, NULLDIR}}; // Bishop, Rook
    Piece pieces[3] = {Pieces::Bishop, Pieces::Rook, Pieces::Queen};

    for (int i = 0; i < 3; i++)
    {
        Bitboard pieces = board->colorBB[board->sideToMove] && board->pieceBB[Pieces::Bishop];
        Bitboard enemyPieces = board->colorBB[board->otherSide];
        while (pieces)
        {
            int bishop = popLSB(&pieces);

            Bitboard bb = generateSlidingMovesBB(&board, bishop, offsets[i], false); // TODO: and to piece BB?
            if (capturesOnly)
            {
                bb &= enemyPieces;
            }
            while (bb)
            {
                int move = popLSB(&bb);
                appendMove(moves, board->getMove(bishop, move));
            }
        }
    }
}

void *generateLegalQueenMoves(bool capturesOnly)
{
    Bitboard queens = board->colorBB[board->sideToMove] && board->pieceBB[Pieces::Queen];
    Bitboard enemyPieces = board->colorBB[board->otherSide];
    while (queens)
    {
        int queen = popLSB(&queens);
        Bitboard bb = generateSlidingMovesBB(&board, queen, queenMoveOffsets, false) & emptyBB;
        if (capturesOnly)
        {
            bb &= enemyPieces;
        }
        while (bb)
        {
            int move = popLSB(&bb);
            appendMove(&moves, board->getMove(queen, move));
        }
    }
}

MoveList *generateLegalNonCaptures()
{
    generateLegalPawnNonCaptures();
    generateLegalKnightNonCaptures();
    generateLegalBishopMoves(false);
    generateLegalRookMoves(false);
    generateLegalQueenMoves(false);
    generateLegalKingNonCaptures();
    return &moves;
}

MoveList *generateMoves()
{
    moves.count = 0;
    generateLegalCaptures();
    generateLegalNonCaptures();
    return &moves;
}
