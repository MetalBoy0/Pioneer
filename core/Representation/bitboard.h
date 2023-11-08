#ifndef BITBOARD_H
#define BITBOARD_H
#include "direction.h"

typedef unsigned long long Bitboard;

// Some helpful bitboard masks

const Bitboard emptyBB = 0ull;

// File masks
extern const Bitboard fileMasks[8];

// Rank masks
extern const Bitboard rankMasks[8];

extern void printBitboard(Bitboard *bb);
extern Bitboard bitboardRay(Direction dir, int square);
extern Bitboard bitboardRay(int from, int to);
extern Bitboard sendRay(Bitboard *bb, Direction dir, int square);
namespace
{

    Bitboard getBitboardFromSquare(int square)
    {
        return 1ull << square;
    }

    // Returns a bool if there is a 1 at the specified square else 0
    bool getBit(Bitboard *bitboard, int square)
    {
        return *bitboard & 1ULL << square;
    }

    void setBit(Bitboard *bitboard, int square)
    {
        *bitboard |= 1ULL << square;
    }

    void clearBit(Bitboard *bitboard, int square)
    {
        *bitboard &= ~(1ULL << square);
    }

    void toggleBit(Bitboard *bitboard, int square)
    {
        *bitboard ^= 1ULL << square;
    }

    int getLSB(Bitboard *bb)
    {
        return __builtin_ctzll(*bb);
    }

    int popLSB(Bitboard *bb)
    {
        int lsb = getLSB(bb);
        clearBit(bb, lsb);
        return lsb;
    }

    int getNumBits(Bitboard *bb)
    {
        return __builtin_popcountll(*bb);
    }
    template <Direction dir>
    Bitboard shift(Bitboard *bb)
    {
        return dir > 0 ? *bb << dir : *bb >> -dir;
    }

    Bitboard shift(Bitboard *bb, Direction dir, int amount)
    {
        return dir > 0 ? *bb << (dir * amount) : *bb >> (-dir * amount);
    }
}
#endif