#ifndef MOVE_H
#define MOVE_H

#include <cstdint>
#include "piece.h"

// Moves will be a 16 bit unsigned integer
typedef uint32_t Move;

// Move format:
// 0000 0000 000000 000000
// ECCP Prom   To    From
// ECCP: En Passant, Castle, Capture, Promotion



constexpr bool isCapture(Move move)
{
    return (move & 0x1000) != 0;
}

constexpr bool isCastle(Move move)
{
    return (move & 0x2000) != 0;
}

constexpr bool isEnPassant(Move move)
{
    return (move & 0x4000) != 0;
}

constexpr bool isPromotion(Move move)
{
    return (move & 0x8000) != 0;
}

constexpr Piece getPromotion(Move move)
{
    return (Piece)((move >> 12) & 0x7);
}

constexpr int getTo(Move move)
{
    return (move >> 6) & 0x3F;
}

constexpr int getFrom(Move move)
{
    return move & 0x3F;
}

constexpr bool isEnPassantCapture(Move move)
{
    return isCapture(move) && isEnPassant(move);
}

#endif