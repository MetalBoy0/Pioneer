#ifndef DIRECTION_H
#define DIRECTION_H
#include <cstdint>
#include <cstdlib>

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
extern int16_t distToEdge[64][8];
extern void initDirections();
extern int getDirIndex(Direction dir);
namespace
{

    Direction getDirectionBetween(int from, int to)
    {
        return directions[from][to];
    }

    bool onEdge(int to)
    {
        return (!distToEdge[to][0] || !distToEdge[to][1] || !distToEdge[to][2] ||
                !distToEdge[to][3] || !distToEdge[to][4] || !distToEdge[to][5] ||
                !distToEdge[to][6] || !distToEdge[to][7]);
    }

}

#endif