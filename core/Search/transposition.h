#ifndef TRANSPOSITION_H
#define TRANSPOSITION_H

#include "..\Representation\move.h"
#include "..\Representation\board.h"

class TranspositionTable
{
public:
    struct Entry
    {
        int zobrist;
        int depth;
        int value;
        Move bestMove;
    };

private:
    Entry *table;
    int size;

public:
    TranspositionTable(int size);
    ~TranspositionTable();

    void store(int zobrist, int depth, int value, Move bestMove);
    Entry probe(int zobrist);
    void clear();

    int getSize() { return size; }
    int getUsed();
};

#endif