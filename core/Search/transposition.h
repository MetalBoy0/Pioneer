#ifndef TRANSPOSITION_H
#define TRANSPOSITION_H

#include "..\Representation\move.h"
#include "..\Representation\board.h"

class TranspositionTable
{
public:
    int failed = INT32_MIN;
    enum EvalType
    {
        Exact,
        Lower,
        Upper,
        NOTINIT
    };
    struct Entry
    {
        int zobrist;
        int depth;
        int value;
        EvalType evalType;
        Move bestMove;
    };

private:
    Entry *table;
    

public:
    TranspositionTable(int size);
    ~TranspositionTable();

    int used = 0;
    int size;

    void store(unsigned long long zobrist, int depth, int value, Move bestMove, EvalType evalType);
    int probe(unsigned long long zobrist, int depth, int alpha, int beta);
    Move getMove(int zobrist);
    void clear();

    int getSize() { return size; }
    int getUsed();
};

#endif