#include "legal.h"
#include "main.h"



bool pawnLegal(Move move) {
    bool isWhite = Piece::getSide(move.movePiece) == Piece::White;
    int skipNum = (isWhite ? 1 : 7);
    int rank = Piece::getRank(move.from);
    int file = Piece::getFile(move.from);
    int forward = (isWhite ? 1 : -1);
    int toRank = Piece::getRank(move.to);
    int toFile = Piece::getFile(move.to);
    if (!move.isCapture) {

        bool canSkip = rank == skipNum;

        // If pawn hasn't moved any direction sideways
        if (rank == toRank) {
            // if pawn has only moved one square
            if (toRank - rank == forward) {
                return true;
            }
                //if pawn has moved two squares and it can skip
            else if (toRank == forward * 2 && canSkip) {
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

bool isLegal(Move move) {
    if (move.movePiece == Piece::Pawn) {
        return pawnLegal(move);
    }
    if (move.movePiece == Piece::Knight) {
        return true; //knightLegal(move, board);
    } else {
        return true;
    }
}

