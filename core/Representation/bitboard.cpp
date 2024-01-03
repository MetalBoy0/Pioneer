#include <iostream>
#include "bitboard.h"
#include <cassert>
using namespace std;

const Bitboard rankMasks[8] = {
    0x00000000000000FF,
    0x000000000000FF00,
    0x0000000000FF0000,
    0x00000000FF000000,
    0x000000FF00000000,
    0x0000FF0000000000,
    0x00FF000000000000,
    0xFF00000000000000};

const Bitboard fileMasks[8] = {
    0x0101010101010101,
    0x0202020202020202,
    0x0404040404040404,
    0x0808080808080808,
    0x1010101010101010,
    0x2020202020202020,
    0x4040404040404040,
    0x8080808080808080};

const Bitboard shortCastle[9] = {
    0b11ull << 61,
    0, 0, 0, 0, 0, 0, 0,
    0b11 << 1};

const Bitboard longCastle[9] = {
    0b1110000ull << 56,
    0, 0, 0, 0, 0, 0, 0,
    0b1110000};

Bitboard knightMoves[64];

Bitboard kingMoves[64];

void initBBs()
{
    // Knights
    for (int i = 0; i < 64; i++)
    {
        Bitboard bb = 0ULL;
        int rank = i / 8;
        int file = i % 8;
        if (rank < 6 && file < 7)
        {
            setBit(&bb, i + 17);
        }
        if (rank < 6 && file > 0)
        {
            setBit(&bb, i + 15);
        }
        if (rank < 7 && file < 6)
        {
            setBit(&bb, i + 10);
        }
        if (rank < 7 && file > 1)
        {
            setBit(&bb, i + 6);
        }
        if (rank > 0 && file < 6)
        {
            setBit(&bb, i - 6);
        }
        if (rank > 0 && file > 1)
        {
            setBit(&bb, i - 10);
        }
        if (rank > 1 && file < 7)
        {
            setBit(&bb, i - 15);
        }
        if (rank > 1 && file > 0)
        {
            setBit(&bb, i - 17);
        }
        knightMoves[i] = bb;
    }
    // Kings
    for (int i = 0; i < 64; i++)
    {
        Bitboard bb = 0ULL;
        int rank = i / 8;
        int file = i % 8;
        if (rank < 7)
        {
            setBit(&bb, i + 8);
        }
        if (rank > 0)
        {
            setBit(&bb, i - 8);
        }
        if (file < 7)
        {
            setBit(&bb, i + 1);
        }
        if (file > 0)
        {
            setBit(&bb, i - 1);
        }
        if (rank < 7 && file < 7)
        {
            setBit(&bb, i + 9);
        }
        if (rank < 7 && file > 0)
        {
            setBit(&bb, i + 7);
        }
        if (rank > 0 && file < 7)
        {
            setBit(&bb, i - 7);
        }
        if (rank > 0 && file > 0)
        {
            setBit(&bb, i - 9);
        }
        kingMoves[i] = bb;
    }
}

void printBitboard(Bitboard *bb)
{
    for (int i = 0; i < 64; i++)
    {
        if (i % 8 == 0)
        {
            cout << endl;
        }
        cout << getBit(bb, i) << " ";
    }
    cout << endl;
}

// A bitboard of all the squares in the ray from the square in the direction
Bitboard bitboardRay(Direction dir, int square)
{
    Bitboard bb = 0ULL;
    int i = square;
    while (i >= 0 && i < 64)
    {
        setBit(&bb, i);
        i += dir;
    }
    return bb;
}

// A bitboard of all the squares in the ray from the square to the square (includes starting square but not ending square   )
Bitboard bitboardRay(int from, int to)
{
    Bitboard bb = 0ULL;
    Direction dir = getDirectionBetween(from, to);
    // assert (from != to);
    if (dir == NULLDIR)
    {
        return 0ULL;
    }
    int i = from;
    while (i >= 0 && i < 64)
    {
        setBit(&bb, i);
        i += dir;
        if (i == to)
        {
            break;
        }
    }
    return bb;
}

Bitboard sendRay(Bitboard *bb, Direction dir, int square)
{
    int to = square + dir;
    while (true)
    {
        if (to < 0 || to > 63)
        {
            if (square == to - dir)
            {
                return 0ULL;
            }
            return bitboardRay(square, to - dir);
        }
        if (getBit(bb, to))
        {
            if (square == to)
            {
                return 0ULL;
            }
            return bitboardRay(square, to);
        }
        to += dir;
    }
}