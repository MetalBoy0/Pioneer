#include "move.h"
#include "../Representation/board.h"





constexpr bool isCastle(Move move)
{
    return ((move >> 19) & 1) != 0;
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

