#ifndef DIRECTION_H
#define DIRECTION_H

enum Direction
{
    NORTH = 8,
    SOUTH = -8,
    EAST = 1,
    WEST = -1,
    NORTH_EAST = 9,
    NORTH_WEST = 7,
    SOUTH_EAST = -7,
    SOUTH_WEST = -9
};

Direction directions[64][64];

Direction getDirectionBetween(int from, int to)
{
    return directions[from][to];
}



#endif