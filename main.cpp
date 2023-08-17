#include <iostream>


using namespace std;


class Piece {

public:
    static const int None = 0;    //0b00000
    static const int Pawn = 1;    //0b00001
    static const int Knight = 2;  //0b00010
    static const int Bishop = 3;  //0b00011
    static const int Rook = 4;    //0b00100
    static const int Queen = 5;   //0b00101
    static const int King = 6;    //0b00110

    //Messy but works, piece val array: Had to insert private in the middle of public
private:
    constexpr static const int pieceVals[7] = {None, Pawn, Knight, Bishop, Rook, Queen, King};

public:
    static const int White = 8;   //0b01000
    static const int Black = 16;  //0b10000


    static int getSide(int piece) {
        return 0b11000 & piece;
    }

    static int getPieceType(int piece) {
        return 0b00111 & piece;
    }

    static int getPieceFromChar(char pieceChar) {
        bool isWhite = isupper(pieceChar);

        //cout << piece << endl;
        int pieceNum;
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
        //cout << piece << endl;
        int pieceVal = pieceVals[pieceNum] | (isWhite ? 0b01000 : 0b10000);
        return pieceVal;


    }

};


class Move {
public:
    int from{};
    int to{};
    int movePiece{};
    int toPiece{};
    bool isWhiteToMove{};
    bool isCapture{};

    Move(int fromSquare, int toSquare, int movePiece, int toPiece) {

        from = fromSquare;
        to = toSquare;
        movePiece = movePiece;
        toPiece = toPiece;
        isCapture = Piece::getPieceType(toPiece) != Piece::None;
        isWhiteToMove = Piece::getSide(movePiece) == Piece::White;
    }

};

// The main board class
class Board {
public:
    int squares[64]{};
    bool isWhiteMove;

    Board() {
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

    void printBoard() {
        cout << "test" << endl;
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

    Move getMove(int from, int to) {
        Move move = *new Move(from, to, squares[from], squares[to]);
        return move;
    }

    void MakeMove(Move move) {
        squares[move.to] = squares[move.from];
        squares[move.from] = Piece::None;

    }
};

int to1dIndex(int x, int y) {
    return y * 8 + x;
}


int main() {
    //Driver Code
    Board board = Board();
    //board.printBoard();

    while (true) {
        // Main input loop
        string input;
        cin >> noskipws >> input;
        if (input == "d") {
            board.printBoard();
        }
        if (input == "makemove") {
            //string coords = input.substr(9,3);
            cout << input << endl;
            //board.MakeMove(move);
        }
    }

    return 0;
}