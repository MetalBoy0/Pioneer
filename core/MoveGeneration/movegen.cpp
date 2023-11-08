#include "movegen.h"

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

MoveList *generateMoves(Board *board, MoveList *moveList)
{
    moveList->count = 0;

    generateLegalNonCaptures(board, moveList);
    generateLegalCaptures(board, moveList);
    return moveList;
}

// Generate all legal castles for the current position
// TODO
MoveList *generateLegalCastles(Board *board, MoveList *moveList)
{
    return moveList;
}

// Generate all legal captures for the current position
MoveList *generateLegalCaptures(Board *board, MoveList *moveList)
{
    generateLegalPawnCaptures(board, moveList);
    generateLegalKnightCaptures(board, moveList);
    generateLegalBishopCaptures(board, moveList);
    generateLegalRookCaptures(board, moveList);
    generateLegalQueenCaptures(board, moveList);
    // generateLegalKingCaptures(board, moveList);
    captures += moveList->count;
    return moveList;
}

// Generate all legal pawn captures for the current position
MoveList *generateLegalPawnCaptures(Board *board, MoveList *MoveList)
{
    // The Idea is to take the pawn bitboard, shift it diagonally, and then AND it with the empty squares
    Bitboard bb = board->pieceBB[Pieces::Pawn] & board->colorBB[board->sideToMove];
    if (board->sideToMove == Pieces::White)
    {
        bb = shift<SE>(&bb);
    }
    else
    {
        bb = shift<NE>(&bb);
    }
    bb &= board->colorBB[board->otherSide];

    // Collect moves from the bitboard
    Direction dir = board->sideToMove == Pieces::White ? SE : NE;
    while (bb)
    {
        int to = popLSB(&bb);
        int from = to - dir;
        if (distIsMoreThanOne(from, to))
        {
            continue;
        }
        appendMove(MoveList, board->getMove(from, to));
    }

    bb = board->pieceBB[Pieces::Pawn] & board->colorBB[board->sideToMove];
    if (board->sideToMove == Pieces::White)
    {
        bb = shift<SW>(&bb);
    }
    else
    {
        bb = shift<NW>(&bb);
    }
    bb &= board->colorBB[board->otherSide];

    // Collect moves from the bitboard
    dir = board->sideToMove == Pieces::White ? SW : NW;
    while (bb)
    {
        int to = popLSB(&bb);
        int from = to - dir;
        if (distIsMoreThanOne(from, to))
        {
            continue;
        }
        appendMove(MoveList, board->getMove(from, to));
    }
    return MoveList;
}

// Generate all legal knight captures for the current position
MoveList *generateLegalKnightCaptures(Board *board, MoveList *MoveList)
{
    // Similar to the knight non captures

    Bitboard bb = board->pieceBB[Pieces::Knight] & board->colorBB[board->sideToMove];
    Direction dir[] = {NNE, NEE, SEE, SSE, SSW, SWW, NWW, NNW};
    for (int i = 0; i < 8; i++)
    {
        Bitboard temp = shift(&bb, dir[i], 1);
        temp &= board->colorBB[board->otherSide];
        while (temp)
        {
            int to = popLSB(&temp);
            int from = to - dir[i];
            if (abs(indexToRank(from) - indexToRank(to)) <= 2 && abs(indexToFile(from) - indexToFile(to)) <= 2 && isValidIndex(to))
            {
                appendMove(MoveList, board->getMove(from, to));
            }
        }
    }

    return MoveList;
}

// Generate all legal bishop captures for the current position
MoveList *generateLegalBishopCaptures(Board *board, MoveList *MoveList)
{
    // Get all bishops
    Bitboard bb = board->pieceBB[Pieces::Bishop] & board->colorBB[board->sideToMove];
    Direction dir[] = {NE, SE, SW, NW};
    while (bb)
    {
        int bishop = popLSB(&bb);
        int from = bishop;
        for (int i = 0; i < 4; i++)
        {
            bishop = from;
            while (true)
            {
                int to = bishop + dir[i];
                if (!isValidIndex(to) || distIsMoreThanOne(bishop, to))
                {
                    break;
                }
                if (getBit(&board->colorBB[board->sideToMove], to))
                {
                    break;
                }
                if (getBit(&board->colorBB[board->otherSide], to))
                {
                    // Add move
                    Move move = board->getMove(from, to);
                    appendMove(MoveList, move);
                }
                bishop = to;
            }
        }
    }

    return MoveList;
}

// Generate all legal rook captures for the current position
MoveList *generateLegalRookCaptures(Board *board, MoveList *MoveList)
{
    // Get all rooks
    Bitboard bb = board->pieceBB[Pieces::Rook] & board->colorBB[board->sideToMove];
    Direction dir[] = {N, E, S, W};
    while (bb)
    {
        int rook = popLSB(&bb);
        int from = rook;
        for (int i = 0; i < 4; i++)
        {
            rook = from;
            while (true)
            {
                int to = rook + dir[i];
                if (!isValidIndex(to) || distIsMoreThanOne(rook, to))
                {
                    break;
                }
                if (getBit(&board->colorBB[board->sideToMove], to))
                {
                    break;
                }
                if (getBit(&board->colorBB[board->otherSide], to))
                {
                    // Add move
                    Move move = board->getMove(from, to);
                    appendMove(MoveList, move);
                }
                rook = to;
            }
        }
    }
    return MoveList;
}

// Generate all legal queen captures for the current position
MoveList *generateLegalQueenCaptures(Board *board, MoveList *MoveList)
{
    // Get all queens
    Bitboard bb = board->pieceBB[Pieces::Queen] & board->colorBB[board->sideToMove];
    Direction dir[] = {N, E, S, W, NE, SE, SW, NW};
    while (bb)
    {
        int queen = popLSB(&bb);
        int from = queen;
        for (int i = 0; i < 8; i++)
        {
            queen = from;

            while (true)
            {
                int to = queen + dir[i];
                if (!isValidIndex(to) || distIsMoreThanOne(queen, to))
                {
                    break;
                }
                if (getBit(&board->colorBB[board->sideToMove], to))
                {
                    break;
                }
                if (getBit(&board->colorBB[board->otherSide], to))
                {
                    // Add move
                    Move move = board->getMove(from, to);
                    appendMove(MoveList, move);
                }
                queen = to;
            }
        }
    }
    return MoveList;
}

// Generate all legal king captures for the current position
MoveList *generateLegalKingCaptures(Board *board, MoveList *MoveList)
{
    return MoveList;
}

// Generate all legal non captures for the current position
MoveList *generateLegalNonCaptures(Board *board, MoveList *moveList)
{
    generateLegalPawnNonCaptures(board, moveList);
    generateLegalKnightNonCaptures(board, moveList);
    generateLegalBishopNonCaptures(board, moveList);
    generateLegalRookNonCaptures(board, moveList);
    generateLegalQueenNonCaptures(board, moveList);
    generateLegalKingNonCaptures(board, moveList);
    return moveList;
}

// Generate all legal pawn non captures for the current position
MoveList *generateLegalPawnNonCaptures(Board *board, MoveList *MoveList)
{
    // The Idea is to take the pawn bitboard, shift it up one, and then AND it with the empty squares
    // This will give us all the squares that the pawn can move to

    Bitboard bb = board->pieceBB[Pieces::Pawn] & board->colorBB[board->sideToMove];
    if (board->sideToMove == Pieces::White)
    {
        bb = shift<S>(&bb);
    }
    else
    {
        bb = shift<N>(&bb);
    }
    bb &= board->pieceBB[Pieces::Empty];

    // Collect moves from the bitboard
    Direction dir = board->sideToMove == Pieces::White ? S : N;
    while (bb)
    {
        int to = popLSB(&bb);
        int from = to - dir;
        appendMove(MoveList, board->getMove(from, to));
    }
    // Pawn Jumps
    // Works in a similar way, but we shift the pawn bitboard up one, then AND it with the empty squares
    // Then shift the result up one again, and AND it with the empty squares
    bb = board->pieceBB[Pieces::Pawn] & board->colorBB[board->sideToMove];
    bb &= rankMasks[board->sideToMove == Pieces::White ? 6 : 1];
    if (bb)
    {
        if (board->sideToMove == Pieces::White)
        {
            bb = shift<S>(&bb);
        }
        else
        {
            bb = shift<N>(&bb);
        }
        bb &= board->pieceBB[Pieces::Empty];
        if (bb)
        {
            if (board->sideToMove == Pieces::White)
            {
                bb = shift<S>(&bb);
            }
            else
            {
                bb = shift<N>(&bb);
            }
            bb &= board->pieceBB[Pieces::Empty];
            while (bb)
            {
                int to = popLSB(&bb);
                int from = to - dir * 2;
                appendMove(MoveList, board->getMove(from, to));
            }
        }
    }
    return MoveList;
}

// Generate all legal knight non captures for the current position
MoveList *generateLegalKnightNonCaptures(Board *board, MoveList *MoveList)
{
    // Similar to the pawns
    Bitboard bb = board->pieceBB[Pieces::Knight] & board->colorBB[board->sideToMove];
    Direction dir[] = {NNE, NEE, SEE, SSE, SSW, SWW, NWW, NNW};
    for (int i = 0; i < 8; i++)
    {
        Bitboard temp = shift(&bb, dir[i], 1);
        temp &= board->pieceBB[Pieces::Empty];
        while (temp)
        {
            int to = popLSB(&temp);
            int from = to - dir[i];
            if (abs(indexToRank(from) - indexToRank(to)) <= 2 && abs(indexToFile(from) - indexToFile(to)) <= 2 && isValidIndex(to))
            {
                appendMove(MoveList, board->getMove(from, to));
            }
        }
    }
    return MoveList;
}

// Generate all legal bishop non captures for the current position
MoveList *generateLegalBishopNonCaptures(Board *board, MoveList *MoveList)
{
    // Get all bishops
    Bitboard bb = board->pieceBB[Pieces::Bishop] & board->colorBB[board->sideToMove];
    Direction dir[] = {NE, SE, SW, NW};
    while (bb)
    {
        int bishop = popLSB(&bb);
        int from = bishop;
        for (int i = 0; i < 4; i++)
        {
            bishop = from;
            while (true)
            {
                int to = bishop + dir[i];
                if (!isValidIndex(to) || getBit(&board->allPiecesBB, to) || distIsMoreThanOne(bishop, to))
                {
                    break;
                }
                // Add move
                Move move = board->getMove(from, to);
                appendMove(MoveList, move);

                bishop = to;
            }
        }
    }

    return MoveList;
}

// Generate all legal rook non captures for the current position
MoveList *generateLegalRookNonCaptures(Board *board, MoveList *MoveList)
{
    // Get all rooks
    Bitboard bb = board->pieceBB[Pieces::Rook] & board->colorBB[board->sideToMove];
    Direction dir[] = {N, E, S, W};
    while (bb)
    {
        int rook = popLSB(&bb);
        int from = rook;
        for (int i = 0; i < 4; i++)
        {
            rook = from;
            while (true)
            {
                int to = rook + dir[i];
                if (!isValidIndex(to) || getBit(&board->allPiecesBB, to) || distIsMoreThanOne(rook, to))
                {
                    break;
                }
                // Add move
                Move move = board->getMove(from, to);
                appendMove(MoveList, move);

                rook = to;
            }
        }
    }
    return MoveList;
}

// Generate all legal queen non captures for the current position
MoveList *generateLegalQueenNonCaptures(Board *board, MoveList *MoveList)
{
    // Get all queens
    Bitboard bb = board->pieceBB[Pieces::Queen] & board->colorBB[board->sideToMove];
    Direction dir[] = {N, E, S, W, NE, SE, SW, NW};
    while (bb)
    {
        int queen = popLSB(&bb);
        int from = queen;
        for (int i = 0; i < 8; i++)
        {
            queen = from;

            while (true)
            {
                int to = queen + dir[i];
                if (!isValidIndex(to) || getBit(&board->allPiecesBB, to) || distIsMoreThanOne(queen, to))
                {
                    break;
                }
                // Add move
                Move move = board->getMove(from, to);
                appendMove(MoveList, move);

                queen = to;
            }
        }
    }
    return MoveList;
}

// Generate all legal king non captures for the current position
MoveList *generateLegalKingNonCaptures(Board *board, MoveList *MoveList)
{
    // Get the king
    Bitboard bb = board->pieceBB[Pieces::King] & board->colorBB[board->sideToMove];
    Direction dir[] = {N, E, S, W, NE, SE, SW, NW};
    while (bb)
    {
        int king = popLSB(&bb);
        int from = king;
        for (int i = 0; i < 8; i++)
        {
            int to = king + dir[i];
            if (!isValidIndex(to) || getBit(&board->allPiecesBB, to))
            {
                continue;
            }
            // Add move
            Move move = board->getMove(from, to);
            appendMove(MoveList, move);
        }
    }
    return MoveList;
}