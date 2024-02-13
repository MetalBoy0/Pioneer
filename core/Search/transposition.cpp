#include "transposition.h"


void TranspositionTable::store(int zobrist, int depth, int value, Move bestMove)
{
    int index = zobrist % size;
    table[index].zobrist = zobrist;
    table[index].depth = depth;
    table[index].value = value;
    table[index].bestMove = bestMove;
}

TranspositionTable::Entry TranspositionTable::probe(int zobrist)
{
    int index = zobrist % size;
    if (table[index].zobrist == zobrist)
    {
        return table[index];
    }
    else
    {
        return Entry();
    }
}

void TranspositionTable::clear()
{
    for (int i = 0; i < size; i++)
    {
        table[i].zobrist = 0;
    }
}

TranspositionTable::TranspositionTable(int size)
{
    this->size = size;
    this->table = new Entry[size];
}

TranspositionTable::~TranspositionTable()
{
    delete[] table;
}

int TranspositionTable::getUsed()
{
    int used = 0;
    for (int i = 0; i < size; i++)
    {
        if (table[i].zobrist % size == i)
        {
            used++;
        }
    }
    return used;
}


