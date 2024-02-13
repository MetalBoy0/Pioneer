#ifndef SEARCH_H
#define SEARCH_H

#include "..\Representation\board.h"

extern unsigned int startPerft(Board board, unsigned int depth);
extern Move startSearch(Board *board, unsigned int depth, int maxTime, int maxNodes, int wtime, int btime);


#endif