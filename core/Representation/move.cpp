#include "move.h"
#include "../Representation/board.h"



constexpr bool isCapture(Move move)
{
    return (move & 0x40000) != 0;
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
