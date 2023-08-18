
#include <iostream>
#include "main.h"
#include "legal.h"

void Board::makeMove(Move move) {
    bool legal = isLegal(move);
    if (legal) {
        squares[move.to] = squares[move.from];
        squares[move.from] = Piece::None;
    }

}


int main() {
    //Driver Code
    Board board = Board();
    board.printBoard();
    //board.printBoard();

    while (true) {
        // Main input loop
        string input;
        getline(cin, input);

        if (input == "d") {
            board.printBoard();
        } else if (input.substr(0, 8) == "makemove") {
            string coords = input.substr(9, 4);
            int x1 = (int) coords[0] - (int) 'a';
            int y1 = (int) coords[1] - (int) '1';
            int x2 = (int) coords[2] - (int) 'a';
            int y2 = (int) coords[3] - (int) '1';
            int fromSquare = to1dIndex(x1, y1);
            int toSquare = to1dIndex(x2, y2);
            Move move = board.getMove(fromSquare, toSquare);
            board.makeMove(move);
        }
    }

    return 0;
}

