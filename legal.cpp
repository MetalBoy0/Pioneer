#include "legal.h"
#include "Pieces.h"
#include "main.h"
#include <iostream>

using namespace std;

bool Rules::checkLegal(Move* move, Board* board) {
    // Make the move and check if own king is in check
    board->makeMove(move);

    bool isWhite = Piece::getSide(move->movePiece) == Piece::White;

    int kingSquare = board->kingSquares[isWhite ? 0 : 1];

    // Check if king is in check

    for (int i = 0; i < 64; i++) {
        int square = board->squares[i];
        if (Piece::getSide(square) != Piece::getSide(move->movePiece)) {
            Move* checkMove = board->getMove(i, kingSquare);
            if (isPsuedoLegal(checkMove, board)) {

                return false;
            }
            delete checkMove;
        }
    }
    board->undoMove(move);
    return true;


}

/* Checks if a pawn move is legal by making sure step 1, 2, and 3 are true
 * 1. Checks if the pawn is moving in the correct direction
 * 2. Checks if the pawn is moving the correct number of squares
 * 3. Checks if the pawn is capturing a piece
 */

bool Rules::pawnLegal(Move* move, Board* board) {
    // Returns true if the move is legal for a pawn
    bool isWhite = Piece::getSide(move->movePiece) == Piece::White;
    int skipNum = (isWhite ? 1 : 6);
    int rank = Piece::getRank(move->from);
    int file = Piece::getFile(move->from);
    int forward = (isWhite ? 1 : -1);
    int toRank = Piece::getRank(move->to);
    int toFile = Piece::getFile(move->to);

    if (!(move->isCapture)) {


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

                // make sure pawn doesn't pass through any pieces
                if (Piece::getPieceType(board->squares[passingSquare] != Piece::None)) {
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

/* Checks if a knight move is legal by making sure step 1 is true
 * 1. Checks the knight's move based off of the rules of chess
 */

bool Rules::knightLegal(Move* move, Board* board) {
    // Returns true if the move is legal for a knight
    int possibleMoves[] = {-17, -15, -10, -6, 6, 10, 15, 17};

    // If the difference between the to and from square is in the possible moves array
    for (int possibleMove: possibleMoves) {
        int delta = move->to - move->from;
        if (delta == possibleMove &&                   // Make sure that the knight isn't wrapping around the board
            abs(Piece::getFile(move->from) - Piece::getFile(move->to)) < 3) {
            return true;
        }
    }


    return false;
}

/*
 * Checks if a bishop move is legal by making sure step 1 and 2 are true
 * 1. Checks if the bishop is moving diagonally
 * 2. Ensures the bishop isn't jumping over any pieces
 */

bool Rules::bishopLegal(Move* move, Board* board) {
    // returns true if the move is legal for a bishop
    int rank = Piece::getRank(move->from);
    int file = Piece::getFile(move->from);
    int toRank = Piece::getRank(move->to);
    int toFile = Piece::getFile(move->to);


    // Make sure bishop is moving diagonally

    if (abs(toRank - rank) == abs(toFile - file)) {
        // Make sure bishop isn't jumping over any pieces
        int rankDir = (toRank - rank) / abs(toRank - rank);
        int fileDir = (toFile - file) / abs(toFile - file);

        int currentRank = rank + rankDir;
        int currentFile = file + fileDir;
        while (currentRank != toRank && currentFile != toFile) {
            int currentSquare = toBoardIndex(currentFile, currentRank);
            if (Piece::getPieceType(board->squares[currentSquare]) != Piece::None) {

                return false;
            }
            currentRank = currentRank + rankDir;
            currentFile = currentFile + fileDir;
        }

        return true;
    }


    return false;
}

/*
 * Checks if a rook move is legal by making sure step 1 and 2 are true
 * 1. Checks if the rook is moving in a straight line
 * 2. Ensures the rook isn't jumping over any pieces
 */

bool Rules::rookLegal(Move* move, Board* board) {
    // Returns true if the move is legal for a rook
    int rank = Piece::getRank(move->from);
    int file = Piece::getFile(move->from);
    int toRank = Piece::getRank(move->to);
    int toFile = Piece::getFile(move->to);

    // Make sure rook is moving in a straight line
    if (toRank == rank || toFile == file) {

        // Make sure rook isn't jumping over any pieces
        int rankDir = (toRank - rank) / abs(toRank - rank);
        int fileDir = (toFile - file) / abs(toFile - file);

        if (file == toFile) {
            fileDir = 0;
        } else if (rank == toRank) {
            rankDir = 0;
        }

        int currentRank = rank + rankDir;
        int currentFile = file + fileDir;
        while (currentRank != toRank || currentFile != toFile) {

            int currentSquare = toBoardIndex(currentFile, currentRank);
            int piece = Piece::getPieceType(board->squares[currentSquare]);
            if (piece != Piece::None) {
                return false;
            }
            currentRank = currentRank + rankDir;
            currentFile = currentFile + fileDir;
        }
        return true;
    }
    return false;


}

/*
Checks if a queen move is legal by checking if the move is legal for a bishop or a rook
*/

bool Rules::queenLegal(Move* move, Board* board) {

    // A queen is just a bishop and a rook combined


    return bishopLegal(move, board) || rookLegal(move, board);


}

/* Checks if a king move is legal by making sure step 1 is true
 * 1. Checks the king's move based off of the rules of chess
 */

inline bool Rules::kingLegal(Move* move, Board* board) {
    // Returns true if the move is legal for a king
    int rank = Piece::getRank(move->from);
    int file = Piece::getFile(move->from);
    int toRank = Piece::getRank(move->to);
    int toFile = Piece::getFile(move->to);
    int possibleMoves[] = {-9, -8, -7, -1, 1, 7, 8, 9};
    // If the difference between the to and from square is in the possible moves array
    for (int possibleMove: possibleMoves) {
        if (move->to - move->from == possibleMove && toRank - rank < 2 && toFile - rank < 2 && toRank - rank > -2 &&
            toFile - rank > -2) {     // Make sure that the king isn't wrapping around the board
            return true;
        }
    }

    return false;

}

/* Checks if a move is psuedo legal by making sure step 1, 2, 3, and 4 are true
 * 1. Checks if the move is in fact a move
 * 2. Checks if the move is being made by the correct player
 * 3. Checks if the move is capturing a piece of the opposite color
 * 4. Checks if the move is legal for the piece type
 */


// Returns true if the move is legal for the piece type
bool Rules::isPsuedoLegal(Move* move, Board* board) {



    //make sure that the piece is in fact a peice and not a blank square
    if (move->movePiece == Piece::None) {
        return false;
    }

    //make sure peice is moving
    if (move->from == move->to) {
        return false;
    }

    // Make sure that the piece is being moved by the correct player
    // If piece isWhite == boardTurn isWhite
    if ((Piece::getSide(move->movePiece) == Piece::White) != board->isWhiteMove) {
        return false;
    }


    // Make sure the piece is not capturing a piece of the same color
    if (Piece::getSide(move->toPiece) == Piece::getSide(move->movePiece)) {
        return false;
    }


    if (Piece::getPieceType(move->movePiece) == Piece::Pawn) {
        return pawnLegal(move, board);
    }
    if (Piece::getPieceType(move->movePiece) == Piece::Knight) {
        return knightLegal(move, board);
    }
    if (Piece::getPieceType(move->movePiece) == Piece::Bishop) {
        return bishopLegal(move, board);
    }
    if (Piece::getPieceType(move->movePiece) == Piece::Rook) {
        return rookLegal(move, board);
    }
    if (Piece::getPieceType(move->movePiece) == Piece::Queen) {
        return queenLegal(move, board);
    }
    if (Piece::getPieceType(move->movePiece) == Piece::King) {
        return kingLegal(move, board);
    } else {
        return false;
    }
}


/* Checks if a move is legal by making sure step 1 and 2 are true
 * 1. Checks if the move is psuedo legal
 * 2. Checks if the move puts the king in check
 */
// Makes sure that a move is legal
bool Rules::isLegal(Move* move, Board* board) {

    //Will piece cause check to own king?
    if (checkLegal(move, board) && isPsuedoLegal(move, board)) {
        return true;
    }
    return false;
}
