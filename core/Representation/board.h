#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include "move.h"
#include "bitboard.h"
#include "piece.h"
using namespace std;

struct pieceList
{
    int pieces[16];
    int board[64];
    int count;
};

struct indexList
{
    int index[64];
    int count;
};

extern void appendBB(indexList *list, Bitboard BB);

class Board
{
public:
    Piece board[64];          // 64 board array
    Pieces::Color sideToMove; // Color of side to move
    Pieces::Color otherSide;
    pieceList pieces[7][2]; // Pieces by type and color [piece][color]
    bool isWhite;           // True if white, false if black
    int score;
    int enPassantSquare; // -1 if no en passant square, otherwise the square
    int ply;             // number of moves since the start of the game
    int removeCastlingRightsWQ = -1;
    int removeCastlingRightsWK = -1;
    int removeCastlingRightsBQ = -1;
    int removeCastlingRightsBK = -1;
    int castleKey;
    unsigned long long pieceHash = 0;
    unsigned long long enPassantHash = 0;
    unsigned long long zobristKey = 0;
    bool inCheck;       // If if the current side to move is in check.
    indexList checkers; // Number of checks

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
    bool isEnPassant(Move move);
    bool isCheck(Move move);
    bool isAttacked(int square, Pieces::Color side);
    Direction isPinned(int square);
    Move getMove(int from, int to, Piece piece = Pieces::Empty, bool isCastle = false);

    // Bitboards
    Bitboard colorBB[9]; // Pieces by color 0 == White, 8 == None
    Bitboard pieceBB[7];
    Bitboard allPiecesBB;
    Bitboard attackedBB[9];
    Bitboard getPieceBB(Piece piece);
    Bitboard checkingBB;

    // History
    Move pastMoves[1000];
    int enPassantHistory[1000];

    // Utils

    void printBoard();                                                                                                                                                                 // Print the board
    void loadFEN(string fen, bool isWhite, bool whiteCanCastleKingSide, bool whiteCanCastleQueenSide, bool blackCanCastleKingSide, bool blackCanCastleQueenSide, int enPassantSquare); // Load a fen string
    void clearBoard();                                                                                                                                                                 // Clear the board
    void setupBitboards();                                                                                                                                                             // Set up the bitboards
    void clearPieceLists();                                                                                                                                                            // Clear the piece lists

    indexList piecesAttackingSquare(int square); // Returns the number of enemy pieces attacking the square
    indexList getCheckers();
    Bitboard getAttackedBB(Pieces::Color side);

    // Constructor
    Board(); // Default constructor
};

namespace PieceSquareTable
{

    const int psq[7][64] = {{0},
                            {// const int pawnTableB[64] = {
                             0, 0, 0, 0, 0, 0, 0, 0,
                             50, 50, 50, 50, 50, 50, 50, 50,
                             10, 10, 20, 30, 30, 20, 10, 10,
                             5, 5, 10, 25, 25, 10, 5, 5,
                             0, 0, 0, 20, 20, 0, 0, 0,
                             5, -5, -10, 0, 0, -10, -5, 5,
                             5, 10, 10, -20, -20, 10, 10, 5,
                             0, 0, 0, 0, 0, 0, 0, 0},
                            {// const int knightsB[64] = {
                             -50, -40, -30, -30, -30, -30, -40, -50,
                             -40, -20, 0, 0, 0, 0, -20, -40,
                             -30, 0, 10, 15, 15, 10, 0, -30,
                             -30, 5, 15, 20, 20, 15, 5, -30,
                             -30, 0, 15, 20, 20, 15, 0, -30,
                             -30, 5, 10, 15, 15, 10, 5, -30,
                             -40, -20, 0, 5, 5, 0, -20, -40,
                             -50, -40, -30, -30, -30, -30, -40, -50},
                            {// const int bishopsB[64] = {
                             -20, -10, -10, -10, -10, -10, -10, -20,
                             -10, 0, 0, 0, 0, 0, 0, -10,
                             -10, 0, 5, 10, 10, 5, 0, -10,
                             -10, 5, 5, 10, 10, 5, 5, -10,
                             -10, 0, 10, 10, 10, 10, 0, -10,
                             -10, 10, 10, 10, 10, 10, 10, -10,
                             -10, 5, 0, 0, 0, 0, 5, -10,
                             -20, -10, -10, -10, -10, -10, -10, -20},
                            {// const int rooksB[64] = {
                             0, 0, 0, 0, 0, 0, 0, 0,
                             5, 10, 10, 10, 10, 10, 10, 5,
                             -5, 0, 0, 0, 0, 0, 0, -5,
                             -5, 0, 0, 0, 0, 0, 0, -5,
                             -5, 0, 0, 0, 0, 0, 0, -5,
                             -5, 0, 0, 0, 0, 0, 0, -5,
                             -5, 0, 0, 0, 0, 0, 0, -5,
                             0, 0, 0, 5, 5, 0, 0, 0},
                            {// const int queensB[64] = {
                             -20, -10, -10, -5, -5, -10, -10, -20,
                             -10, 0, 0, 0, 0, 0, 0, -10,
                             -10, 0, 5, 5, 5, 5, 0, -10,
                             -5, 0, 5, 5, 5, 5, 0, -5,
                             0, 0, 5, 5, 5, 5, 0, -5,
                             -10, 5, 5, 5, 5, 5, 0, -10,
                             -10, 0, 5, 0, 0, 0, 0, -10,
                             -20, -10, -10, -5, -5, -10, -10, -20},

                            {// const int kingsB[64] = {
                             -80, -70, -70, -70, -70, -70, -70, -80,
                             -60, -60, -60, -60, -60, -60, -60, -60,
                             -40, -50, -50, -60, -60, -50, -50, -40,
                             -30, -40, -40, -50, -50, -40, -40, -30,
                             -20, -30, -30, -40, -40, -30, -30, -20,
                             -10, -20, -20, -20, -20, -20, -20, -10,
                             20, 20, -5, -5, -5, -5, 20, 20,
                             20, 30, 10, 0, 0, 10, 30, 20}};

}

namespace
{
    string startFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
    void addPiece(pieceList *list, int square)
    {
        list->pieces[list->count] = square;
        list->board[square] = list->count;
        list->count++;
    }

    void removePiece(pieceList *list, int square)
    {
        int index = list->board[square];
        list->pieces[index] = list->pieces[list->count - 1];
        list->board[list->pieces[index]] = index;
        list->count--;
    }

    void movePieceList(pieceList *list, int from, int to)
    {
        int index = list->board[from];
        list->pieces[index] = to;
        list->board[to] = index;
    }

    int getPiece(pieceList *list, int square)
    {
        return list->pieces[list->board[square]];
    }

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
    int invertY(int index)
    {
        return indexToRank(index) * 8 + indexToFile(index);
    }
}

class RNG
{
private:
    unsigned long long seed;

public:
    int rand64()
    {
        seed += seed/2 + 78651276235ULL;
        seed ^= (seed >> 15);
        seed ^= (seed << 32);
        seed ^= ((seed + 78651276235ULL) >> 21);
        return seed * 893652645892356ULL;
    }
};
namespace Zobrist
{
    extern unsigned long long piece[12][64];
    extern unsigned long long side[2];
    extern unsigned long long castle[16];
    extern unsigned long long enPassant[8];
    extern void setup();
}

#endif