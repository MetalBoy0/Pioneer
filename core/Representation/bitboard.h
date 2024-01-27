#ifndef BITBOARD_H
#define BITBOARD_H
#include "direction.h"
#include "piece.h"
typedef unsigned long long Bitboard;

// Some helpful bitboard masks

const Bitboard emptyBB = 0ull;
const Bitboard fullBB = ~emptyBB;

// castling masks
extern const Bitboard shortCastle[9];
extern const Bitboard longCastle[10];

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
extern Bitboard dirToBB[8][64];

extern void printBitboard(Bitboard *bb);
extern Bitboard bitboardRay(Direction dir, int square);
extern Bitboard bitboardRay(int from, int to);
extern inline Bitboard sendRay(Bitboard *bb, Direction dir, int square);
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

    template <Piece>
    Bitboard getAttackBB(int s);
    template <Piece>
    Bitboard getAttackBB(int s, Bitboard *squares);

    template <>
    Bitboard getAttackBB<Pieces::Pawn>(int s)
    {
    }

    template <>
    Bitboard getAttackBB<Pieces::Knight>(int s)
    {
        return knightMoves[s];
    }

    template <>
    Bitboard getAttackBB<Pieces::Bishop>(int s, Bitboard *squares)
    {
        Bitboard attackBB = 0;
        for (auto x : bishopDirections)
        {
            if (distToEdge[s][getDirIndex(x)] == 0)
            {
                continue;
            }
            attackBB |= sendRay(squares, x, s);
        }
        return attackBB;
    }

    template <>
    Bitboard getAttackBB<Pieces::Rook>(int s, Bitboard *squares)
    {
        Bitboard attackBB = 0;
        for (auto x : rookDirections)
        {
            if (distToEdge[s][getDirIndex(x)] == 0)
            {
                continue;
            }
            attackBB |= sendRay(squares, x, s);
        }
        return attackBB;
    }

    template <>
    Bitboard getAttackBB<Pieces::Queen>(int s, Bitboard *squares)
    {
    }

    template <>
    Bitboard getAttackBB<Pieces::King>(int s)
    {
        return kingMoves[s];
    }
}
#endif