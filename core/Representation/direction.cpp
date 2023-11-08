#include "direction.h"

Direction directions[64][64];

// Init the direction array
void initDirections()
{
    for (int i = 0; i < 64; i++)
    {
        for (int j = 0; j < 64; j++)
        {
            directions[i][j] = NULLDIR;
        }
    }
    for (int i = 0; i < 64; i++)
    {
        int rank = i / 8;
        int file = i % 8;
        for (int b = 0; b < 64; b++)
        {
            int rank2 = b / 8;
            int file2 = b % 8;
            if (i == b)
            {
                directions[i][b] = NULLDIR;
                continue;
            }
            if (rank == rank2 && file != file2)
            {
                directions[i][b] = file2 > file ? E : W;
            }
            else if (rank != rank2 && file == file2)
            {
                directions[i][b] = rank2 > rank ? N : S;
            }
            else if (rank2 - rank == file2 - file)
            {
                directions[i][b] = rank2 > rank ? (file2 > file ? NE : NW) : (file2 > file ? SE : SW);
            }
            else if (rank2 - rank == file - file2)
            {
                directions[i][b] = rank2 > rank ? (file2 > file ? NE : NW) : (file2 > file ? SE : SW);
            }
            else if (rank2 - rank == 2 && file2 - file == 1)
            {
                directions[i][b] = NNE;
            }
            else if (rank2 - rank == 1 && file2 - file == 2)
            {
                directions[i][b] = NEE;
            }
            else if (rank2 - rank == -1 && file2 - file == 2)
            {
                directions[i][b] = SEE;
            }
            else if (rank2 - rank == -2 && file2 - file == 1)
            {
                directions[i][b] = SSE;
            }
            else if (rank2 - rank == -2 && file2 - file == -1)
            {
                directions[i][b] = SSW;
            }
            else if (rank2 - rank == -1 && file2 - file == -2)
            {
                directions[i][b] = SWW;
            }
            else if (rank2 - rank == 1 && file2 - file == -2)
            {
                directions[i][b] = NWW;
            }
            else if (rank2 - rank == 2 && file2 - file == -1)
            {
                directions[i][b] = NNW;
            }
            else if (rank2 - rank == 1 && file2 - file == 0)
            {
                directions[i][b] = NN;
            }
            else if (rank2 - rank == -1 && file2 - file == 0)
            {
                directions[i][b] = SS;
            }
            else
            {
                directions[i][b] = NULLDIR;
            }
        }
    }
}