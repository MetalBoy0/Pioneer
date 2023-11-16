#include "search.h"
#include "../MoveGeneration/movegen.h"
#include "../../uci.h"

int perft(Board *board, unsigned int depth, unsigned int ply)
{

    MoveList moveList;
    generateMoves(board, &moveList);

    if (depth == 0)
    {
        return 1;
    }

    int nodes = 0;

    for (int i = 0; i < moveList.count; i++)
    {
        board->makeMove(moveList.moves[i]);
        nodes += perft(board, depth - 1, ply + 1);
        board->undoMove();
    }

    return nodes;
}

int startPerft(Board board, unsigned int depth)
{
    if (depth <= 0)
    {
        return 1;
    }
    int nodes = 0;

    MoveList moveList;
    moveList.count = 0;
    board.
    int newNodes = 0;
    for (int i = 0; i < moveList.count; i++)
    {
        board.makeMove(moveList.moves[i]);
        newNodes = perft(&board, depth - 1, 0);
        board.undoMove();
        nodes += newNodes;
        cout << moveToString(moveList.moves[i]) << ": " << newNodes << "\n";
    }
    return nodes;
}

Move startSearch(Board *board, unsigned int depth)
{
    return 0;
}