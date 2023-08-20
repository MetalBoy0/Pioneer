//
// Created by Will on 8/18/2023.
//

#include "pieces.h"
#include <iostream>

using namespace std;

int Piece::getPieceFromChar(char pieceChar) {
    bool isWhite = isupper(pieceChar);

    int pieceNum;
    int pieceVals[7] = {None, Pawn, Knight, Bishop, Rook, Queen, King};
    switch (pieceChar) {
        case 'p':
            pieceNum = 1;
            break;
        case 'n':
            pieceNum = 2;
            break;
        case 'b':
            pieceNum = 3;
            break;
        case 'r': // rook
            pieceNum = 4;
            break;
        case 'q':
            pieceNum = 5;
            break;
        case 'k':
            pieceNum = 6;
            break;
        case 'P':
            pieceNum = 1;
            break;
        case 'N':
            pieceNum = 2;
            break;
        case 'B':
            pieceNum = 3;
            break;
        case 'R': // rook
            pieceNum = 4;
            break;
        case 'Q':
            pieceNum = 5;
            break;
        case 'K':
            pieceNum = 6;
            break;
        default:
            throw invalid_argument("Unknown piece type " + to_string(pieceChar));
    }

    int pieceVal = pieceVals[pieceNum] | (isWhite ? 0b01000 : 0b10000);
    return pieceVal;
}