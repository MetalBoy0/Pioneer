#ifndef BITBOARD_H
#define BITBOARD_H
#include "direction.h"

typedef unsigned long long Bitboard;

// Some helpful bitboard masks

const Bitboard emptyBB = 0ull;
const Bitboard fullBB = ~emptyBB;

//castling masks
extern const Bitboard shortCastle[9];
extern const Bitboard longCastle[9];

// File masks
extern const Bitboard fileMasks[8];
extern const Bitboard middleMask;

// Rank masks
extern const Bitboard rankMasks[8];

// Knight moves
extern Bitboard knightMoves[64];

// King moves
extern Bitboard kingMoves[64];

// Direction BBs
extern Bitboard diagonalsBB[64];
extern Bitboard straightsBB[64];
extern Bitboard canSeeBB[64];

extern void printBitboard(Bitboard *bb);
extern Bitboard bitboardRay(Direction dir, int square);
extern Bitboard bitboardRay(int from, int to);
extern Bitboard sendRay(Bitboard *bb, Direction dir, int square);
extern void initBBs();
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