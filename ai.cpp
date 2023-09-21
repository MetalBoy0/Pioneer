//
// Created by Will on 8/20/2023.
//
#include <iostream>
#include "ai.h"
#include "time.h"
#include "list"

using namespace std;

Move *BESTMOVE = nullptr;
float BESTVAL = INT_MIN;
bool ISWHITE;
int DEPTH = 0;
int CUTTOFFS = 0;
int NODES;

int PieceVals[] = {0, 1, 3, 3, 5, 7, 100};

inline float eval(Board *board);

float eval(Board *board, Move *move);

// Sorts the move list by the value of the move
void sortMove(Board *board, list<Move *> moves) {

    moves.sort([board](Move *a, Move *b) {
        return eval(board, board->getMove(a->from, a->to)) < eval(board, board->getMove(b->from, b->to));
    });
}

// Quiescence search
inline float quiessence(Board *board, int ply, float alpha, float beta) {

    float standpat = eval(board);
    if (standpat >= beta) {
        return beta;
    }
    if (alpha < standpat) {
        alpha = standpat;
    }
    if (ply > 5) {
        return standpat;
    }
    list<Move *> legalMoves = board->generateLegalMoves();
    //sortMove(board, legalMoves);
    for (auto x: legalMoves) {
        if (x->isCapture) {
            board->makeMove(x);
            float score = -quiessence(board, ply + 1, -beta, -alpha);
            board->undoMove(x);
            if (score >= beta) {
                delete x;
                return beta;
            }
            if (score > alpha) {
                alpha = score;
            }
        }
        delete x;
    }
    return alpha;
}

// Minimax search with alpha beta pruning
inline float minMax(Board *board, int depth, int ply, float alpha, float beta) {

    NODES++;
    if (depth == 0) {

        float value = eval(board);

        return value;
    }

    list<Move *> legalMoves = board->generateLegalMoves();
    //sortMove(board, legalMoves);
    for (auto x: legalMoves) {
        board->makeMove(x);
        float value = -minMax(board, depth - 1, ply + 1, -beta, -alpha);

        board->undoMove(x);
        if (value >= beta) {
            CUTTOFFS++;
            delete x;
            return value;

        }

        if (value > alpha) {
            alpha = value;
            if (ply == 0) {
                if (BESTMOVE != nullptr) {
                    delete BESTMOVE;
                }
                BESTMOVE = x;
                char x1 = Piece::getFile(x->from) + 'a';
                char y1 = Piece::getRank(x->from) + '1';
                char x2 = Piece::getFile(x->to) + 'a';
                char y2 = Piece::getRank(x->to) + '1';
                cout << "Bestmove: " << x1 << y1 << x2 << y2 << endl;
            }

        } else {
            delete x;
        }


    }

    return alpha;

}


// Evaluate the board
inline float eval(Board* board) {

    // sum of all pieces
    float boardVal = 0;
    list<Move*> legalMoves = board->generateLegalMoves();
    // Detect checkmate

    if (legalMoves.empty()) {
        if (board->isWhiteMove) {
            return INT_MIN + 1 * (ISWHITE ? 1 : -1);
        } else {
            return INT_MAX - 1 * (ISWHITE ? 1 : -1);
        }
    }

    for (int i = 0; i < 64; i++) {
        int x = board->squares[i];
        bool isWhite = Piece::getSide(x) == Piece::White;
        int type = Piece::getPieceType(x);
        int rank = Piece::getRank(i);
        int file = Piece::getFile(i);
        // reward for moving pawns up the board
        if (type == Piece::Pawn) {

            if (isWhite) {
                boardVal += rank * 0.1f;
            } else {
                boardVal -= (7 - rank) * 0.1f;
            }


        }
        // multiplier for pieces in the center 4 files of the board
        if (file > 2 && file < 6) {
            if (isWhite) {
                boardVal += 0.1f;
            } else {
                boardVal -= 0.1f;
            }
        }

        int sideMulti = isWhite == board->isWhiteMove ? 1 : -1;
        boardVal += PieceVals[type] * sideMulti;


    }
    //reward for having more legal moves than opponent

    float legalMovesVal = legalMoves.size() * 0.03f;


    return boardVal + legalMovesVal;

}

// Evaluate a move
float eval(Board *board, Move *move) {

    board->makeMove(move);
    float value = eval(board);
    board->undoMove(move);
    return value;
}

// Generates the best move found by the ai
Move *findBestMove(Board *board) {

    ISWHITE = board->isWhiteMove;
    clock_t start = clock();
    DEPTH = 0;
    Move *NULL_MOVE = nullptr;
    while (((clock() - start) * 1000) / CLOCKS_PER_SEC < 6000) {
        NODES = 0;
        CUTTOFFS = 0;
        DEPTH++;
        BESTVAL = 0;
        int perspective = ISWHITE ? 1 : -1;
        if (ISWHITE) {
            BESTVAL = minMax(board, DEPTH, 0, INT_MIN, INT_MAX);
        } else {
            BESTVAL = minMax(board, DEPTH, 0, INT_MIN, INT_MAX);
        }
        cout << "DEPTH: " << DEPTH << " Nodes Searched: " << NODES << " Nodes Pruned: " << CUTTOFFS << " Best Val: "
             << BESTVAL << endl;


    }

    cout << "Time Elapsed: " << ((clock() - start) * 1000) / CLOCKS_PER_SEC << "ms" << endl;


    return BESTMOVE;
}
