#include "movegen.h"
#include "../Representation/piece.h"

int captures = 0;
int checks = 0;

bool distIsMoreThanOne(int from, int to)
{
    return (abs(indexToRank(from) - indexToRank(to)) > 1) || (abs(indexToFile(from) - indexToFile(to)) > 1);
}

void generateCheckBB(Board *board)
{
    board->checkingBB = -1;
    Bitboard king = board->pieceBB[Pieces::King] & board->colorBB[board->sideToMove];
    int kingIndex = popLSB(&king);
}

bool legalKnightMove(int from, int to)
{
    int rankF = indexToRank(from);
    int fileF = indexToFile(from);
    int rankT = indexToRank(to);
    int fileT = indexToFile(to);
    if (abs(rankF - rankT) == 2 && abs(fileF - fileT) == 1)
    {
        return true;
    }
    if (abs(rankF - rankT) == 1 && abs(fileF - fileT) == 2)
    {
        return true;
    }
    return false;
}

void appendMove(MoveList *moveList, Move move)
{
    moveList->moves[moveList->count] = move;
    moveList->count++;
}

void generatePawnMoves(Board *board, MoveList *MoveList, bool onlyCaptures)
{
    // Shift the pawn BBs to get the moves
    Bitboard pawns = board->pieceBB[Pieces::Pawn] & board->colorBB[board->sideToMove];
    Bitboard moves = 0;
    Bitboard captureE = 0;
    Bitboard captureW = 0;

    if (!onlyCaptures)
    {
        if (board->sideToMove == Pieces::Color::White)
        {
            moves |= shift<S>(&pawns);
        }
        else
        {
            moves |= shift<N>(&pawns);
        }
    }
    if (board->sideToMove == Pieces::Color::White)
    {
        captureE |= shift<SE>(&pawns);
        captureW |= shift<SW>(&pawns);
    }
    else
    {
        captureE |= shift<NE>(&pawns);
        captureW |= shift<NW>(&pawns);
    }

    moves &= board->pieceBB[0];
    // Add en passant to each of these
    Bitboard mask = (board->colorBB[board->otherSide]);
    if (board->enPassantSquare != -1)
    {
        mask |= (1ULL << board->enPassantSquare);
    }
    captureE &= mask;
    captureW &= mask;

    // Decode BBs into moves
    while (moves)
    {
        // TODO: Deal with pins
        int to = popLSB(&moves);
        int from = to + (board->sideToMove == Pieces::Color::White ? 8 : -8);
        if (indexToRank(to) == 0 || indexToRank(to) == 7)
        {
            appendMove(MoveList, board->getMove(from, to, Pieces::Queen));
            appendMove(MoveList, board->getMove(from, to, Pieces::Rook));
            appendMove(MoveList, board->getMove(from, to, Pieces::Bishop));
            appendMove(MoveList, board->getMove(from, to, Pieces::Knight));
        }
        else
        {
            appendMove(MoveList, board->getMove(from, to));
        }
        // Double pawn push
        if (indexToRank(from) == 1 || indexToRank(from) == 6)
        {
            int to2 = to - (board->sideToMove == Pieces::Color::White ? 8 : -8);
            if (board->board[to2] == Pieces::Empty)
            {
                appendMove(MoveList, board->getMove(from, to2));
            }
        }
    }
    while (captureE)
    {
        int to = popLSB(&captureE);
        int from = to + (board->sideToMove == Pieces::Color::White ? 7 : -9);
        if (abs(indexToFile(to) - indexToFile(from)) >= 2)
        {
            continue;
        }
        if (indexToRank(to) == 0 || indexToRank(to) == 7)
        {
            appendMove(MoveList, board->getMove(from, to, Pieces::Queen));
            appendMove(MoveList, board->getMove(from, to, Pieces::Rook));
            appendMove(MoveList, board->getMove(from, to, Pieces::Bishop));
            appendMove(MoveList, board->getMove(from, to, Pieces::Knight));
        }
        else
        {
            appendMove(MoveList, board->getMove(from, to));
        }
    }
    while (captureW)
    {
        int to = popLSB(&captureW);
        int from = to + (board->sideToMove == Pieces::Color::White ? 9 : -7);
        if (abs(indexToFile(to) - indexToFile(from)) >= 2)
        {
            continue;
        }
        if (indexToRank(to) == 0 || indexToRank(to) == 7)
        {
            appendMove(MoveList, board->getMove(from, to, Pieces::Queen));
            appendMove(MoveList, board->getMove(from, to, Pieces::Rook));
            appendMove(MoveList, board->getMove(from, to, Pieces::Bishop));
            appendMove(MoveList, board->getMove(from, to, Pieces::Knight));
        }
        else
        {
            appendMove(MoveList, board->getMove(from, to));
        }
    }
}

void generateKnightMoves(Board *board, MoveList *moveList, bool onlyCaptures)
{
    Bitboard knights = board->pieceBB[Pieces::Knight] & board->colorBB[board->sideToMove];
    while (knights)
    {
        int knight = popLSB(&knights);
        Bitboard moves = knightMoves[knight];
        moves &= board->colorBB[board->otherSide] | (onlyCaptures ? 0 : board->pieceBB[0]);
        while (moves)
        {
            int to = popLSB(&moves);
            appendMove(moveList, board->getMove(knight, to));
        }
    }
}

void generateSlidingRays(Board *board, MoveList *moveList, int from, int startDir, int endDir, bool onlyCaptures)
{
    const Direction directions[8] = {N, S, E, W, NE, NW, SE, SW};
    for (int i = startDir; i <= endDir; i++)
    {
        Direction dir = directions[i];
        int oldF = indexToFile(from);
        int oldR = indexToRank(from);
        int to = from + dir;
        int toF = indexToFile(to);
        int toR = indexToRank(to);
        if (abs(toF - oldF) <= 1 && abs(toR - oldR) <= 1 && to >= 0 && to < 64)
        {

            while (board->board[to] == Pieces::Empty && to >= 0 && to < 64)
            {

                if ((abs(toF - oldF) > 1) || (abs(toR - oldR) > 1))
                {
                    break;
                }

                if (!onlyCaptures)
                {
                    appendMove(moveList, board->getMove(from, to));
                }

                oldF = indexToFile(to);
                oldR = indexToRank(to);
                to += dir;
                toF = indexToFile(to);
                toR = indexToRank(to);
            }
        }
        if (board->board[to] != Pieces::Empty &&
            Pieces::getColor(board->board[to]) != board->sideToMove &&
            (!((abs(toF - oldF) > 1) ||
               (abs(toR - oldR) > 1))) &&
            to >= 0 && to < 64)
        {
            appendMove(moveList, board->getMove(from, to));
        }
    }
}

void generateSlidingMoves(Board *board, MoveList *movelist, bool onlyCaptures)
{
    Bitboard rooks = board->pieceBB[Pieces::Rook] & board->colorBB[board->sideToMove];
    Bitboard bishops = board->pieceBB[Pieces::Bishop] & board->colorBB[board->sideToMove];
    Bitboard queens = board->pieceBB[Pieces::Queen] & board->colorBB[board->sideToMove];
    while (rooks)
    {
        int rook = popLSB(&rooks);
        generateSlidingRays(board, movelist, rook, 0, 3, onlyCaptures);
    }
    while (bishops)
    {
        int bishop = popLSB(&bishops);
        generateSlidingRays(board, movelist, bishop, 4, 7, onlyCaptures);
    }
    while (queens)
    {
        int queen = popLSB(&queens);
        generateSlidingRays(board, movelist, queen, 0, 7, onlyCaptures);
    }
}

void generateCastles(Board *board, MoveList *MoveList)
{
    // TODO: King in check
    // Pieces in the way
    Bitboard attacked = board->attackedBB[board->otherSide];
    if ((shortCastle[board->sideToMove] & board->allPiecesBB) == 0 && board->whiteCanCastleKingSide && ~board->checkingBB == 0)
    {
        if (!getBit(&attacked, 61) && !getBit(&attacked, 62))
        {
            appendMove(MoveList, board->getMove(60, 62, Pieces::Empty, true));
        }
    }
}

void generateKingMoves(Board *board, MoveList *MoveList)
{
    Bitboard king = board->pieceBB[Pieces::King] & board->colorBB[board->sideToMove];
    while (king)
    {
        int kingIndex = popLSB(&king);
        Bitboard attacked = board->attackedBB[board->otherSide];
        Bitboard moves = kingMoves[kingIndex];
        moves &= (board->colorBB[board->otherSide] | board->pieceBB[0]) & ~attacked;
        while (moves)
        {
            int to = popLSB(&moves);
            appendMove(MoveList, board->getMove(kingIndex, to));
        }
    }
}

void generateMoves(Board *board, MoveList *moveList, bool onlyCaptures)
{
    moveList->count = 0;

    generateCheckBB(board);

    // Generate moves for each piece

    generatePawnMoves(board, moveList, onlyCaptures);
    generateKnightMoves(board, moveList, onlyCaptures);
    generateSlidingMoves(board, moveList, onlyCaptures);
    generateKingMoves(board, moveList);
    generateCastles(board, moveList);
    if (!onlyCaptures)
    {
        // generateCastles(board, moveList);
    }
}
