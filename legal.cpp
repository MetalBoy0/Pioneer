#include "legal.h"
#include "Pieces.h"
#include "main.h"
#include <iostream>

using namespace std;




bool Rules::pawnLegal(Move move, Board board) {

    bool isWhite = Piece::getSide(move.movePiece) == Piece::White;
    int skipNum = (isWhite ? 6 : 1);
    int rank = Piece::getRank(move.from);
    int file = Piece::getFile(move.from);
    int forward = (isWhite ? -1 : 1);
    int toRank = Piece::getRank(move.to);
    int toFile = Piece::getFile(move.to);

    if (!move.isCapture) {


        // If pawn hasn't moved any direction sideways
        if (file == toFile) {
            bool canSkip = rank == skipNum;
            // if pawn has only moved one square
            if (toRank - rank == forward) {

                return true;
            }

                //if pawn has moved two squares and it can skip
            else if (toRank - rank == forward * 2 && canSkip) {
                // Find passing square
                int passingSquare = toBoardIndex(file, rank + forward);
                cout << passingSquare << endl;
                if (Piece::getPieceType(board.squares[passingSquare] != Piece::None)) {
                    return false;
                }
                return true;
            }
        }
    }
        // Pawn is capturing:
        //Check if its moving diagonal and forwards
    else if (toRank - rank == forward && file != toFile) {
        return true;
    }
    return false;

}

bool Rules::knightLegal(Move move, Board board) {
    bool isWhite = Piece::getSide(move.movePiece) == Piece::White;
    int rank = Piece::getRank(move.from);
    int file = Piece::getFile(move.from);
    int toRank = Piece::getRank(move.to);
    int toFile = Piece::getFile(move.to);

    return true;
}
bool Rules::bishopLegal(Move move, Board board) {return true;}
bool Rules::rookLegal(Move move, Board board) {return true;}
bool Rules::queenLegal(Move move, Board board) {return true;}
bool Rules::kingLegal(Move move, Board board) {return true;}

bool Rules::isLegal(Move move, Board board) {
    if (Piece::getPieceType(move.movePiece) == Piece::Pawn) {
        return Rules::pawnLegal(move, board);
    }
    if (Piece::getPieceType(move.movePiece) == Piece::Knight) {
        return knightLegal(move, board);
    } else {
        return true;
    }
}
