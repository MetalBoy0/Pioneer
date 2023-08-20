//
// Created by WVGarrison on 8/18/2023.
//

#ifndef CHESSENGINE_LEGAL_H
#define CHESSENGINE_LEGAL_H

class Move;
class Board;

class Rules {
public:
    static bool pawnLegal(Move move, Board board);
    static bool knightLegal(Move move, Board board);
    static bool bishopLegal(Move move, Board board);
    static bool rookLegal(Move move, Board board);
    static bool queenLegal(Move move, Board board);
    static bool kingLegal(Move move, Board board);
    static bool isLegal(Move move, Board board);
    static bool checkLegal(Move move, Board board);
    static bool isPsuedoLegal(Move move, Board board);
};


#endif //CHESSENGINE_LEGAL_H
