#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include "move.h"
#include "bitboard.h"
using namespace std;

struct indexList
{
    int index[64];
    int count;
};

extern void appendBB(indexList *list, Bitboard BB);

class Board
{
public:
    int board[64];            // 64 board array
    Pieces::Color sideToMove; // Color of side to move
    Pieces::Color otherSide;
    bool isWhite;        // True if white, false if black
    int enPassantSquare; // -1 if no en passant square, otherwise the square
    int ply;             // number of moves since the start of the game
    indexList checks;    // Number of checks

    // Castling rights
    bool whiteCanCastleKingSide;
    bool whiteCanCastleQueenSide;
    bool blackCanCastleKingSide;
    bool blackCanCastleQueenSide;

    // Move Handling
    void makeMove(Move move);
    void undoMove();
    void setMove(Move move);
    void revertSetMove(Move move);
    Move getMove(int from, int to, Piece piece = Pieces::Empty, bool isCastle = false);

    // Bitboards
    Bitboard colorBB[9]; // Pieces by color 0 == White, 8 == None
    Bitboard pieceBB[7];
    Bitboard allPiecesBB;
    template <Piece piece>
    Bitboard getPieceBB(Board board);
    Bitboard checkingBB;

    // History
    Move pastMoves[1000];
    int enPassantHistory[1000];

    // Utils

    void printBoard();        // Print the board
    void loadFEN(string fen); // Load a fen string
    void clearBoard();        // Clear the board
    void setupBitboards();    // Set up the bitboards

    indexList piecesAttackingSquare(int square); // Returns the number of enemy pieces attacking the square
    indexList getCheckers();
    Bitboard getAttackedBB();

    // Constructor
    Board(); // Default constructor
};

namespace
{
    string startFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
    int indexToRank(int index)
    {
        return 7 - index / 8;
    }
    int indexToFile(int index)
    {
        return index % 8;
    }
    int rankFileToIndex(int rank, int file)
    {
        return (7 - rank) * 8 + file;
    }
    int isValidIndex(int index)
    {
        return index >= 0 && index < 64;
    }

}

#endif