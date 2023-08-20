//
// Created by Will on 8/20/2023.
//
#include "iostream"
#include "ai.h"

using namespace std;

Move BESTMOVE = *new Move();
int BESTVAL = INT_MIN;
bool ISWHITE;
int DEPTH = 3;
int NODES;
int PieceVals[] = {0, 1, 3, 3, 5, 7, 100};

int eval(Board board);

int minMax(Board board, int depth, int ply) {
    NODES++;
    if (depth == 0) {
        return eval(board);
    }
    list<Move> legalMoves = board.generateLegalMoves();
    int bestVal;
    if (board.isWhiteMove) {
        bestVal = INT_MIN;
    } else {
        bestVal = INT_MAX;
    }

    Move bestmove;
    for (auto x: legalMoves) {
        board.makeMove(x);
        int value = -minMax(board, depth - 1, ply + 1);
        board.undoMove();
        if (board.isWhiteMove) {
            if (ply == 0 && value > bestVal) {
                bestmove = x.copy();
            }
            bestVal = max(bestVal, value);

        } else {
            if (ply == 0 && value < bestVal) {
                bestmove = x;
            }
            bestVal = min(bestVal, value);
        }
    }
    if (ply == 0) {
        BESTMOVE = bestmove;
    }
    return bestVal;

}

int eval(Board board) {
    // sum of all pieces
    int boardVal = 0;
    for (auto x: board.squares) {
        bool isWhite = Piece::getSide(x) == Piece::White;
        int type = Piece::getPieceType(x);

        int sideMulti = isWhite ? 1 : -1;
        boardVal += PieceVals[type] * sideMulti;

    }
    return boardVal;

}


Move findBestMove(Board board) {
    ISWHITE = board.isWhiteMove;
    NODES = 0;
    minMax(board, DEPTH, 0);
    cout << "Nodes Searched: " << NODES << endl;
    return BESTMOVE;

}

