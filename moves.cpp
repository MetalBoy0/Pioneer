//
// Created by Will on 8/18/2023.
//

#include "moves.h"


Move::Move(int fromSquare, int toSquare, int movePiece, int toPiece) {
    from = fromSquare;
    to = toSquare;
    this->movePiece = movePiece;
    this->toPiece = toPiece;
    isCapture = Piece::getPieceType(toPiece) != Piece::None;
    isWhiteToMove = Piece::getSide(movePiece) == Piece::White;
    if (Piece::getPieceType(movePiece) == Piece::Pawn) {
        promotion = Piece::getRank(to) == 0 || Piece::getRank(to) == 7;
    } else {
        promotion = false;
    }
}

Move::Move() {
    // Initialize member variables with default values or leave them uninitialized
    from = 0;
    to = 0;
    movePiece = 0;
    toPiece = 0;
    isCapture = false;
    isWhiteToMove = false;
    promotion = false;
}

Move Move::copy() {
    return *new Move(this->from, to, movePiece, toPiece);
}
