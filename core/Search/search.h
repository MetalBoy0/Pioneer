#ifndef SEARCH_H
#define SEARCH_H

#include "..\Representation\board.h"

extern int startPerft(Board board, unsigned int depth);
extern Move startSearch(Board *board, unsigned int depth);
#endif