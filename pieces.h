//
// Created by Will on 8/18/2023.
//

#ifndef CHESSENGINE_PIECES_H
#define CHESSENGINE_PIECES_H

// A class that stores the piece types and colors
class Piece {

public:
    static const int None = 0;    //0b00000
    static const int Pawn = 1;    //0b00001
    static const int Knight = 2;  //0b00010
    static const int Bishop = 3;  //0b00011
    static const int Rook = 4;    //0b00100
    static const int Queen = 5;   //0b00101
    static const int King = 6;    //0b00110

    static const int White = 8;   //0b01000
    static const int Black = 16;  //0b10000
    inline static int getSide(int piece) {
        return 0b11000 & piece;
    }

    inline static int getPieceType(int piece) {
        return 0b00111 & piece;
    }

    inline static int getRank(int square) {
        return 7-(int) square / 8;
    }

    inline static int getFile(int square) {
        return square % 8;
    }

    static int getPieceFromChar(char pieceChar);

};


#endif //CHESSENGINE_PIECES_H
