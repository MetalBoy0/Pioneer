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
    const int White = 8;   //0b01000
    static const int Black = 16;  //0b10000


    static int getSide(int piece) {
        return 0b11000 & piece;
    }

    static int getPieceType(int piece) {
        return 0b00111 & piece;
    }

    static int getPieceFromChar(char piece) {
        bool isWhite = isupper(piece);
        tolower(piece);
        cout << piece << endl;
        int pieceNum;
        switch (piece) {
            case 'p':
                pieceNum = 0;
                break;
            case 'n':
                pieceNum = 1;
                break;
            case 'b':
                pieceNum = 2;
                break;
            case 'r':
                pieceNum = 3;
                break;
            case 'q':
                pieceNum = 4;
                break;
            case 'k':
                pieceNum = 5;
                break;
            default:
                throw invalid_argument("Uknown piece type " + to_string(piece));
        }
        cout << piece << endl;
        int pieceVal = pieceVals[pieceNum];


    }

};


// The main board class
class Board {
public:
    int squares[64];
    bool isWhiteMove;

    Board() {
        // Setup squares list
        string fenpos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
        int current = 0;
        for (char fenChar: fenpos) {

            // If char is a / (new chess row)
            if ('/' == fenChar) {
                continue;
            }
            // If char is a number (x amount of empty spaces)
            if (isdigit(fenChar)) {
                for (int i = 0; i < (int) fenChar; i++) {

                    squares[current] = Piece::None;
                    current++;
                }
                continue;
            }
            // If char is a piece

            current++;

        }

    }
};


// Piece Class




int main() {
    //Driver Code
    Board board;

    return 0;
}