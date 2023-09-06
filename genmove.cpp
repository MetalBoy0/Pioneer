//
// Created by WVGarrison on 9/5/2023.
//

#include "genmove.h"
#include "main.h"
#include "iostream"

using namespace std;

list<Move*> Board::generateLegalMoves() {

    list<Move*> moves;
    for (int square = 0; square < 64; square++) {
        int piece = squares[square];

        if (piece != Piece::None && Piece::getSide(piece) == (isWhiteMove ? Piece::White : Piece::Black)) {

            if (Piece::getPieceType(piece) == Piece::Pawn) {
                    // find forward 3 squares
                    int forward = (Piece::getSide(piece) == Piece::White ? -1 : 1);
                    int possibleSquares[] = {8, 7, 9, 16};
                    for (int possibleSquare: possibleSquares) {
                    int toSquare = square + possibleSquare * forward;
                    if (toSquare >= 0 && toSquare < 64) {
                        Move* move = getMove(square, toSquare);
                        if (Rules::isLegal(move, this)) {
                            moves.push_back(move);
                        }
                    }
                }
                continue;
            }
            if (Piece::getPieceType(piece) == Piece::Knight) {
                int possibleSquares[] = {6, 10, 15, 17, -6, -10, -15, -17};
                for (int possibleSquare: possibleSquares) {
                    int toSquare = square + possibleSquare;
                    if (toSquare >= 0 && toSquare < 64) {
                        Move* move = getMove(square, toSquare);
                        if (Rules::isLegal(move, this)) {
                            moves.push_back(move);
                        }
                    }
                }
                continue;
            }
            if (Piece::getPieceType(piece) == Piece::Bishop) {
                int possibleSquares[] = {7, 9, -7, -9};
                for (int possibleSquare: possibleSquares) {
                    int toSquare = square + possibleSquare;
                    while (toSquare >= 0 && toSquare < 64) {
                        Move* move = getMove(square, toSquare);
                        if (Rules::isLegal(move, this)) {
                            moves.push_back(move);
                        }
                        if (Piece::getPieceType(squares[toSquare]) != Piece::None) {
                            break;
                        }
                        toSquare += possibleSquare;
                    }
                }
                continue;
            }
            if (Piece::getPieceType(piece) == Piece::Rook) {
                int possibleSquares[] = {8, -8, 1, -1};
                for (int possibleSquare: possibleSquares) {
                    int toSquare = square + possibleSquare;
                    while (toSquare >= 0 && toSquare < 64) {
                        Move* move = getMove(square, toSquare);
                        if (Rules::isLegal(move, this)) {
                            moves.push_back(move);
                        }
                        if (Piece::getPieceType(squares[toSquare]) != Piece::None) {
                            break;
                        }
                        toSquare += possibleSquare;
                    }
                }
                continue;
            }
            if (Piece::getPieceType(piece) == Piece::Queen) {
                int possibleSquares[] = {7, 8, 9, 1, -7, -8, -9, -1};
                for (int possibleSquare: possibleSquares) {
                    int toSquare = square + possibleSquare;
                    while (toSquare >= 0 && toSquare < 64) {
                        Move* move = getMove(square, toSquare);
                        if (Rules::isLegal(move, this)) {
                            moves.push_back(move);
                        }
                        if (Piece::getPieceType(squares[toSquare]) != Piece::None) {
                            break;
                        }
                        toSquare += possibleSquare;
                    }
                }
                continue;
            }
            if (Piece::getPieceType(piece) == Piece::King) {
                int possibleSquares[] = {7, 8, 9, 1, -7, -8, -9, -1};
                for (int possibleSquare: possibleSquares) {
                    int toSquare = square + possibleSquare;
                    if (toSquare >= 0 && toSquare < 64) {
                        Move* move = getMove(square, toSquare);
                        if (Rules::isLegal(move, this)) {
                            moves.push_back(move);
                        }
                    }
                }
                continue;
            }
            for (int i = 0; i < 64; i++) {
                Move* move = getMove(square, i);
                if (Rules::isLegal(move, this)) {
                    moves.push_back(move);
                }
            }


        }

    }
    return moves;
}

