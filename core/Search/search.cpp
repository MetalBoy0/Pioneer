#include "search.h"
#include "../MoveGeneration/movegen.h"
#include "../../uci.h"
#include <cassert>




unsigned int perft(Board *board, unsigned int depth, unsigned int ply)
{
    MoveList moveList;


    generateMoves(board, &moveList);
    if (depth == 1)
    {
        return moveList.count;
    }
    if (depth == 0)
    {
        return 1;
    }
    unsigned int nodes = 0;

    // DEBUG

    for (int i = 0; i < moveList.count; i++)
    {
        board->makeMove(moveList.moves[i]);
        nodes += perft(board, depth - 1, ply + 1);
        board->undoMove();
    }

    return nodes;
}

unsigned int startPerft(Board board, unsigned int depth)
{
    MoveList moveList;
    generateMoves(&board, &moveList);

    unsigned int nodes = 0;

    for (int i = 0; i < moveList.count; i++)
    {
        board.makeMove(moveList.moves[i]);
        unsigned int c = perft(&board, depth - 1, 1);
        board.undoMove();
        nodes += c;
        cout << moveToString(moveList.moves[i]) << ": " << c << "\n";
    }
    cout << "Total nodes: " << nodes << "\n";
    return nodes;
}

Move startSearch(Board *board, unsigned int depth)
{
    return 0;
}