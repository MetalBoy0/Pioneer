#include "search.h"
#include "../MoveGeneration/movegen.h"
#include "../../uci.h"
#include <cassert>

int perft(Board *board, unsigned int depth, unsigned int ply)
{
    Bitboard origin = board->pieceBB[Pieces::Empty];
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
    int nodes = 0;

    // DEBUG

    for (int i = 0; i < moveList.count; i++)
    {
        if (origin != board->pieceBB[Pieces::Empty] && false)
        {
            cout << "ERROR: Bitboards changed" << endl;
            cout << "Move: " << moveToString(moveList.moves[i - 1]) << endl;
            cout << "Index: " << i - 1 << endl;
            cout << "Depth: " << depth << endl;
            cout << "Ply: " << ply << endl;
            cout << "Origin: " << origin << endl;
            cout << "Current: " << board->pieceBB[Pieces::Empty] << endl;
            assert(false);
        }
        board->makeMove(moveList.moves[i]);
        nodes += perft(board, depth - 1, ply + 1);
        board->undoMove();
    }

    
    return nodes;
}

int startPerft(Board board, unsigned int depth)
{
    MoveList moveList;
    generateMoves(&board, &moveList);

    int nodes = 0;

    for (int i = 0; i < moveList.count; i++)
    {
        board.makeMove(moveList.moves[i]);
        int c = perft(&board, depth - 1, 1);
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