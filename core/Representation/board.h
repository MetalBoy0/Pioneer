#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include "move.h"
#include "bitboard.h"

using namespace std;

class Board
{
public:
    int board[64];       // 64 board array
    int sideToMove;      // false for white, true for black
    int enPassantSquare; // -1 if no en passant square, otherwise the square
    int ply;             // number of moves since the start of the game

    // Castling rights
    bool whiteCanCastleKingSide;
    bool whiteCanCastleQueenSide;
    bool blackCanCastleKingSide;
    bool blackCanCastleQueenSide;

    // Move Handling
    void makeMove(Move move);
    void undoMove(Move move);
    void getMove(int from, int to);
    void getMove(int from, int to, Piece promotion);

    // Bitboards
    Bitboard colorBB[8];
    Bitboard pieceBB[7];
    Bitboard allPiecesBB;
    template <Piece piece>
    Bitboard getPieceBB(Board board);

    // History
    Move pastMoves[1000];
    int enPassantHistory[1000];

    // Utils

    void printBoard();        // Print the board
    void loadFEN(string fen); // Load a fen string
    void clearBoard();        // Clear the board

    // Constructor
    Board(); // Default constructor
};

#endif