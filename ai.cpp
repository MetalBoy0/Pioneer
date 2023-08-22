//
// Created by Will on 8/20/2023.
//
#include <iostream>
#include "ai.h"
#include "time.h"
#include "list"

using namespace std;

Move BESTMOVE = *new Move();
int BESTVAL = INT_MIN;
bool ISWHITE;
int DEPTH = 0;
int CUTTOFFS = 0;
int NODES;

int PieceVals[] = {0, 1, 3, 3, 5, 7, 100};

int eval(Board board);
int eval(Board board, Move move);
void sortMove(Board board, list<Move> moves) {
    moves.sort([](const Move& a, const Move& b) {
        return a.isCapture < b.isCapture;
    });

}

int quiessence(Board board, int alpha, int beta) {
    int standPat = eval(board);
    if (standPat >= beta) {
        return beta;
    }
    if (alpha < standPat) {
        alpha = standPat;
    }
    list<Move> legalMoves = board.generateLegalMoves();
    sortMove(board, legalMoves);
    for (auto x: legalMoves) {
        if (x.isCapture) {
            board.makeMove(x);
            int score = -quiessence(board, -beta, -alpha);
            board.undoMove();
            if (score >= beta) {
                return beta;
            }
            if (score > alpha) {
                alpha = score;
            }
        }
    }
    return alpha;
}

int minMax(Board board, Move thisMove, int depth, int ply, int alpha, int beta) {
    NODES++;
    if (depth == 0) {
        if (board.isWhiteMove) {
            return -quiessence(board, -beta, -alpha);
        }
        int value = quiessence(board, alpha, beta);
        return value;
    }
    list<Move> legalMoves = board.generateLegalMoves();
    sortMove(board, legalMoves);
    if (legalMoves.empty()) {

        cout << "No legal moves" << endl;
        return ISWHITE ? INT_MIN : INT_MAX;
    }
    for (auto x: legalMoves) {
        board.makeMove(x);
        int value = -minMax(board, x, depth - 1, ply + 1, -beta, -alpha);

        board.undoMove();
        if (value >= beta) {
            CUTTOFFS++;
            return beta;
        }
        if (value > alpha) {
            alpha = value;
            if (ply == 0) {
                BESTMOVE = x;
                BESTVAL = value;
            }

        }


    }

    return alpha;

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

int eval(Move move, Board board) {
    board.makeMove(move);
    int value = eval(board);
    board.undoMove();
    return value;
}


Move findBestMove(Board board) {
    ISWHITE = board.isWhiteMove;
    clock_t start = clock();

    while (((clock() - start) * 1000) / CLOCKS_PER_SEC < 3000) {
        NODES = 0;
        CUTTOFFS = 0;
        DEPTH++;
        BESTVAL = 0;
        minMax(board, Move(), DEPTH, 0, INT_MIN, INT_MAX);
        cout << "Nodes Searched: " << NODES << " Nodes Pruned: " << CUTTOFFS << " Best Val: " << BESTVAL << endl;


    }
    cout << "Time Elapsed: " << ((clock() - start) * 1000) / CLOCKS_PER_SEC << "ms" << endl;



    return BESTMOVE;
}
