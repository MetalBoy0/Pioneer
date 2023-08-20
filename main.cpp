
#include <iostream>
#include "main.h"
#include "legal.h"
#include "moves.h"
#include "ai.h"
int toBoardIndex(int x, int y) {
    // invert x
    x = 7 - x;
    return 63 - (y * 8 + x);
}

void Board::updatePieceLists() {
    for (int i = 0; i < 64; i++) {
        int square = squares[i];
        if (Piece::getPieceType(square) == Piece::King) {
            if (Piece::getSide(square) == Piece::White) {
                kingSquares[0] = i;
            } else {
                kingSquares[1] = i;
            }
        }
    }
}

list<Move> Board::generateLegalMoves() {

    list<Move> moves;
    for (int square = 0; square < 64; square++) {
        int piece = squares[square];

        if (piece != Piece::None) {
            for (int toSquare = 0; toSquare < 64; toSquare++) {
                int toPiece = squares[toSquare];

                if (Piece::getSide(toPiece) != Piece::getSide(piece)) {


                    Move move = getMove(square, toSquare);
                    if (Rules::isLegal(move, *this)) {
                        moves.push_front(move);
                    }
                }
            }
        }


    }
    return moves;
}

void Board::updateBoard() {
    // Other player's turn
    isWhiteMove = !isWhiteMove;

    // Update piece lists
    updatePieceLists();
}

void Board::makeMove(Move move) {
    // Add move to past moves list

    pastMoves[pastMoveIndex] = move;
    pastMoveIndex = pastMoveIndex + 1;

    // Preform move

    squares[move.to] = squares[move.from];
    squares[move.from] = Piece::None;

    // if pawn promotion
    if (move.promotion) {
        squares[move.to] = Piece::Queen | Piece::getSide(move.movePiece);
    }
    updateBoard();
}

void Board::printBoard() {

    for (int i = 0; i < 64; i++) {
        int square = squares[i];
        string pieces[] = {" . ", " p ", " n ", " b ", " r ", " q ", " k ", " P ", " N ", " B ", " R ", " Q ",
                           " K "};
        int pieceIndex = 0;
        pieceIndex += Piece::getPieceType(square);
        if (Piece::getSide(square) == Piece::White) {
            pieceIndex = pieceIndex + 6;
        }
        string pieceChar = pieces[pieceIndex];
        cout << pieceChar;
        // Newline if next row of chess board
        if ((i + 1) % 8 == 0) {
            cout << endl;
        }
    }
}

void Board::undoMove() {
    if (pastMoveIndex > 0) {
        pastMoveIndex--;
        Move move = pastMoves[pastMoveIndex];
        squares[move.from] = move.movePiece;
        squares[move.to] = move.toPiece;
        updateBoard();
    }
}

Board::Board() {

    isWhiteMove = false; // will get reverse back to true at the end of this function by updateBoard()
    // Setup past moves list
    for (int i = 0; i < 100; i++) {
        pastMoves[i] = *new Move();
    }

    // Setup squares list
    string fenpos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
    isWhiteMove = true;

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
    // Setup legal moves list
    updateBoard();
}


int main() {
    //Driver Code
    Board board = Board();
    const list<Move> list = board.generateLegalMoves();
    cout << list.size() << endl;
    for (auto x : list) {
        char x1 = Piece::getFile(x.from)+'a';
        char y1 = Piece::getRank(x.from)+'1';
        char x2 = Piece::getFile(x.to)+'a';
        char y2 = Piece::getRank(x.to)+'1';
        cout << x1 << y1 << x2 << y2 << endl;
    }


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

            bool legal = Rules::isLegal(move, board);
            if (legal) {
                board.makeMove(move);
            }
        } else if (input == "undomove") {
            board.undoMove();
        } else if (input == "exit" || input == "quit") {
            break;
        } else if (input == "go") {
            Move bestMove=findBestMove(board);
            char x1 = Piece::getFile(bestMove.from)+'a';
            char y1 = Piece::getRank(bestMove.from)+'1';
            char x2 = Piece::getFile(bestMove.to)+'a';
            char y2 = Piece::getRank(bestMove.to)+'1';
            cout << "Bestmove: "<< x1 << y1 << x2 << y2 << endl;
        }
    }

    return 0;
}


