#include "search.h"
#include "../MoveGeneration/movegen.h"
#include "../../uci.h"
#include <cassert>

struct BoardSnapshot
{
    Bitboard pieceBB[7];
    Bitboard colorBB[9];
    Bitboard checkingBB;
    Piece board[64];
};

bool isSnapshotBoard(BoardSnapshot *snapshot, Board *board)
{
    for (int i = 0; i < 64; i++)
    {
        if (snapshot->board[i] != board->board[i])
        {
            return false;
        }
    }
    for (int i = 0; i < 7; i++)
    {
        if (snapshot->pieceBB[i] != board->pieceBB[i])
        {
            return false;
        }
    }
    if (snapshot->colorBB[0] != board->colorBB[0] || snapshot->colorBB[8] != board->colorBB[8])
    {
        return false;
    }
    return true;
}

unsigned int perft(Board *board, unsigned int depth, unsigned int ply)
{
    Bitboard origin = board->pieceBB[Pieces::Empty];
    MoveList moveList;
    BoardSnapshot snapshot;

    for (int i = 0; i < 64; i++)
    {
        snapshot.board[i] = board->board[i];
    }
    for (int i = 0; i < 7; i++)
    {
        snapshot.pieceBB[i] = board->pieceBB[i];
        
    }
    for (int i = 0; i < 9; i++)
    {
        snapshot.colorBB[i] = board->colorBB[i];
    }
    snapshot.checkingBB = board->checkingBB;
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
        if (!isSnapshotBoard(&snapshot, board))
        {
            cout << "ERROR: Snapshot board does not match current board\n"
                 << endl;
            cout << "Depth: " << depth << " On move: " << moveToString(moveList.moves[i - 1]) << " Ply: " << ply << endl;
        }
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