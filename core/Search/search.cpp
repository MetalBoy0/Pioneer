#include "search.h"
#include "../Representation/board.h"
#include "../Representation/move.h"
#include "../MoveGeneration/movegen.h"
#include "../../uci.h"
#include "evaluate.h"
#include "moveOrder.h"
#include <cassert>
#include <chrono>

using namespace std;

#define MAX_DEPTH 64
#define POSINF 1000000
#define NEGINF -1000000

float min(float a, float b)
{
    return a < b ? a : b;
}
float max(float a, float b)
{
    return a > b ? a : b;
}

struct searchDiagnostics
{
    unsigned int nodes;
    unsigned int qNodes;
    unsigned int time;
    unsigned int cutoffs;
};

searchDiagnostics diagnostics;

struct MoveVal
{
    Move move;
    float value;
};

MoveVal bestMove;
Move startMove;
MoveList path;

bool IsMate(int score)
{
    return abs(score) > POSINF - MAX_DEPTH;
}

float qsearch(Board *board, int ply, float alpha, float beta)
{
    diagnostics.qNodes++;
    int eval = evaluate(board);
    diagnostics.nodes++;
    if (eval >= beta)
    {
        diagnostics.cutoffs++;
        return beta;
    }
    if (eval > alpha)
    {
        alpha = eval;
    }
    MoveList moveList;
    generateMoves(board, &moveList, true);
    sortMoves(&moveList, 0, board, true);
    if (moveList.count == 0)
    {
        return eval;
    }
    for (int i = 0; i < moveList.count; i++)
    {
        board->makeMove(moveList.moves[i]);
        float value = -qsearch(board, ply + 1, -beta, -alpha);
        board->undoMove();
        if (value >= beta)
        {
            return beta;
        }
        if (value > alpha)
        {
            alpha = value;
        }
    }
    return alpha;
}

float search(Board *board, unsigned int depth, int ply, float alpha, float beta)
{
    diagnostics.nodes++;
    if (depth == 0)
    {
        return qsearch(board, ply, alpha, beta);
    }
    if (ply > 0)
    {
        alpha = max(alpha, -POSINF + ply);
        beta = min(beta, POSINF - ply);
        if (alpha >= beta)
        {
            diagnostics.cutoffs++;
            return alpha;
        }
    }
    MoveList moveList;
    generateMoves(board, &moveList);

    if (moveList.count == 0)
    {
        if (board->inCheck)
        {
            return NEGINF + ply;
        }
        else
        {
            return 0;
        }
    }
    sortMoves(&moveList, startMove, board);
    for (int i = 0; i < moveList.count; i++)
    {
        Move move = moveList.moves[i];

        board->makeMove(move);
        float value = -search(board, depth - 1, ply + 1, -beta, -alpha);
        board->undoMove();
        if (value >= beta)
        {
            return beta;
        }
        if (value > alpha)
        {
            alpha = value;
            if (ply == 0)
            {
                bestMove.move = move;
                bestMove.value = value;
            }
            path.moves[ply] = move;
        }
    }
    return alpha;
}

void startIterativeDeepening(Board *board, unsigned int maxDepth, int maxTime = 0, int maxNodes = 0)
{
    diagnostics.nodes = 0;
    diagnostics.qNodes = 0;
    diagnostics.time = 0;
    diagnostics.cutoffs = 0;
    startMove = 0;
    int startTime = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
    for (int i = 1; i <= maxDepth; i++)
    {
        int startTime = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
        bestMove.move = 0;
        bestMove.value = -100000;
        search(board, i, 0, NEGINF, POSINF);
        int currentTime = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
        cout << "info depth " << i << " score cp " << bestMove.value << " nodes " << diagnostics.nodes << " nps " << diagnostics.nodes * 1000 / (currentTime - startTime + 1) << " time " << currentTime - startTime << " pv\n";
        startMove = bestMove.move;
        if (IsMate(bestMove.value))
        {
            break;
        }

        if ((currentTime - startTime) > maxTime && maxTime != 0)
        {
            break;
        }
        if (diagnostics.nodes > maxNodes && maxNodes != 0)
        {
            break;
        }
    }
}

Move startSearch(Board *board, unsigned int depth, int maxTime, int maxNodes, int wtime, int btime)
{
    if (wtime != 0 && btime != 0)
    {
        maxTime = (wtime + btime) / 4;
    }
    bestMove.value = -100000;
    bestMove.move = 0;
    startIterativeDeepening(board, depth, maxTime, maxNodes);
    path.count = depth;
    cout << "\n";
    return bestMove.move;
}

unsigned int perft(Board *board, unsigned int depth)
{
    MoveList moveList;
    generateMoves(board, &moveList);
    if (depth == 1)
    {
        return moveList.count;
    }
    else if (depth == 0)
    {
        return 1;
    }

    unsigned int nodes = 0;
    for (int i = 0; i < moveList.count; i++)
    {
        Move move = moveList.moves[i];
        board->makeMove(move);
        nodes += perft(board, depth - 1);
        board->undoMove();
    }
    return nodes;
}

unsigned int startPerft(Board board, unsigned int depth)
{
    unsigned int nodes = 0;
    MoveList moveList;
    generateMoves(&board, &moveList);
    for (int i = 0; i < moveList.count; i++)
    {
        Move move = moveList.moves[i];
        board.makeMove(move);
        int mNode = perft(&board, depth - 1);
        board.undoMove();
        cout << moveToString(move) << ": " << mNode << "\n";
        nodes += mNode;
    }
    return nodes;
}