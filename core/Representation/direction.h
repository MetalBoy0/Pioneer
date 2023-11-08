#ifndef DIRECTION_H
#define DIRECTION_H

enum Direction
{
    NULLDIR = 0,
    N = 8,
    S = -8,
    E = 1,
    W = -1,
    NE = 9,
    NW = 7,
    SE = -7,
    SW = -9,
    NN = 16, // For pawns
    SS = -16,
    NNE = 17, // For knights
    NEE = 10,
    SEE = -6,
    SSE = -15,
    SSW = -17,
    SWW = -10,
    NWW = 6,
    NNW = 15
};
extern Direction directions[64][64];
extern void initDirections();
namespace
{

    Direction getDirectionBetween(int from, int to)
    {
        return directions[from][to];
    }

}

#endif