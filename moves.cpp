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
}
