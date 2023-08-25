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
    moves.sort([](const Move &a, const Move &b) {
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

        int value = eval(board);

        return value;
    }

    list<Move> legalMoves = board.generateLegalMoves();
    sortMove(board, legalMoves);
    for (auto x: legalMoves) {
        board.makeMove(x);
        int value = -minMax(board, x, depth - 1, ply + 1, -beta, -alpha);

        board.undoMove();
        if (value >= beta) {
            CUTTOFFS++;
            return value;
        }

        if (value > alpha) {
            alpha = value;
            if (ply == 0) {
                BESTMOVE = x;
                if (ply == 0) {
                    char x1 = Piece::getFile(x.from) + 'a';
                    char y1 = Piece::getRank(x.from) + '1';
                    char x2 = Piece::getFile(x.to) + 'a';
                    char y2 = Piece::getRank(x.to) + '1';
                    cout << "Bestmove: " << x1 << y1 << x2 << y2 << endl;
                }
            }

        }


    }

    return alpha;

}

int eval(Board board) {
    // sum of all pieces
    float boardVal = 0;
    list<Move> legalMoves = board.generateLegalMoves();
    // Detect checkmate

    if (legalMoves.size() == 0) {
        if (board.isWhiteMove) {
            return INT_MIN + 1 * (ISWHITE ? 1 : -1);
        } else {
            return INT_MAX - 1 * (ISWHITE ? 1 : -1);
        }
    }

    for (int i = 0; i < 64; i++) {
        int x = board.squares[i];
        bool isWhite = Piece::getSide(x) == Piece::White;
        int type = Piece::getPieceType(x);
        int sideMulti = isWhite == board.isWhiteMove ? 1 : -1;
        boardVal += PieceVals[type] * sideMulti;

    }
    //reward for having more legal moves than opponent

    float legalMovesVal = legalMoves.size() * 0.03;


    return boardVal + legalMovesVal;

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
    DEPTH = 0;
    while (((clock() - start) * 1000) / CLOCKS_PER_SEC < 600000) {
        NODES = 0;
        CUTTOFFS = 0;
        DEPTH++;
        BESTVAL = 0;
        int perspective = ISWHITE ? 1 : -1;
        if (ISWHITE) {
            BESTVAL = minMax(board, *new Move(), DEPTH, 0, INT_MIN, INT_MAX);
        } else {
            BESTVAL = minMax(board, *new Move(), DEPTH, 0, INT_MIN, INT_MAX);
        }
        cout << "DEPTH: " << DEPTH << "Nodes Searched: " << NODES << " Nodes Pruned: " << CUTTOFFS << " Best Val: "
             << BESTVAL << endl;


    }

    cout << "Time Elapsed: " << ((clock() - start) * 1000) / CLOCKS_PER_SEC << "ms" << endl;


    return BESTMOVE;
}
