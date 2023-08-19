
#include <iostream>
#include "main.h"
#include "legal.h"
#include "moves.h"

int toBoardIndex(int x, int y) {
    // invert x
    x = 7 - x;
    return -(y * 8 + x - 63);
}


void Board::makeMove(Move move) {
    bool legal = Rules::isLegal(move, *this);
    if (legal) {
        squares[move.to] = squares[move.from];
        squares[move.from] = Piece::None;
    }

}


Board::Board() {
    // Setup squares list
    string fenpos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
    isWhiteMove = true;
    cout << fenpos << endl;
    int current = 0;
    for (char fenChar: fenpos) {
        // If char is a / (new chess row)
        if ('/' == fenChar) {
            continue;
        }
        // If char is a number (x amount of empty spaces)
        // Stuck here in this loop for some reason :/
        if (fenChar >= '0' && fenChar <= '9') {
            for (int i = 0; i < (int) fenChar - (int) '0'; i++) {
                squares[current] = Piece::None;
                current = current + 1;
            }
            continue;
        }
        // If char is a piece
        squares[current] = Piece::getPieceFromChar(fenChar);
        current = current + 1;
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
            int fromSquare = toBoardIndex(x1, y1);
            int toSquare = toBoardIndex(x2, y2);
            Move move = board.getMove(fromSquare, toSquare);
            cout << move.from << endl;
            board.makeMove(move);
        }
    }

    return 0;
}



