// Handles functions between the user and the engine relating to the UCI protocol

#include <iostream>
#include <sstream>
#include "uci.h"
#include "core\representation\bitboard.h"
#include "core\Search\search.h"
#include <chrono>
#include "core\MoveGeneration\movegen.h"
#include "microbench\microbench.h"
#define MAX_INT -1u;
#define MAX_DEPTH 8;

using namespace std;

Board board;

void setup()
{
    board = Board();

    // Preform several benchmarking tests here:
}

Move stringToMove(string moveString, Board board)
{
    bool castle = false;
    bool promote = Pieces::Empty;
    int to;
    int from;
    if (moveString == "O-O")
    {
        from = 60;
        to = 62;
        castle = true;
    }
    else if (moveString.length() < 4)
    {
        return 0;
    }
    else
    {
        from = ((moveString[0] - 'a') + (8 * (7 - (moveString[1] - '1'))));
        to = ((moveString[2] - 'a') + (8 * (7 - (moveString[3] - '1'))));
    }

    return board.getMove(from, to, promote, castle);
}

string moveToString(Move move)
{
    string moveString = "";
    if (isCastle(move))
    {
        if (indexToFile(move >> 6 & 0x3F) < 4)
        {
            return "O-O-O";
        }
        else
        {
            return "O-O";
        }
    }
    else
    {
        moveString += (char)(indexToFile(move & 0x3F) + 'a');
        moveString += (char)(indexToRank(move & 0x3F) + '1');
        moveString += (char)(indexToFile(move >> 6 & 0x3F) + 'a');
        moveString += (char)(indexToRank(move >> 6 & 0x3F) + '1');
    }
    return moveString;
}

void parseUCI(istringstream &parser)
{
    cout << "id name Pioneer V0.3\n";
    cout << "id author Will Garrison\n";
    cout << "uciok\n";
}

void parseIsReady(istringstream &parser)
{

    cout << "readyok\n";
}

void parseNewGame(istringstream &parser)
{
}

void parsePosition(istringstream &parser)
{
    string input;
    parser >> input;

    if (input == "startpos")
    {
        board.loadFEN(startFen);
    }
    else if (input == "fen")
    {
        parser >> input;
        board.loadFEN(input);
    }
}

// Starts a search by the engine based off the specified parameters
// TODO: Implement wtime and btime
void parseGo(istringstream &parser)
{
    bool perft = false;             // Is a perft search
    unsigned int depthValue = 0;    // Targeted depth of the search
    unsigned int nodesCount = 0;    // Max number of nodes to search
    unsigned int moveTimeValue = 0; // Max time to search in milliseconds

    while (parser)
    {
        string option;
        parser >> option;
        if (option == "perft")
        {
            // Is a perft search
            perft = true;

            // Set depth of perft search
            string depthInput;
            parser >> depthInput;
            depthValue = stoi(depthInput);
        }
        else if (option == "depth")
        {
            // User specified depth
            string depthInput;
            parser >> depthInput;
            depthValue = stoi(depthInput);
        }
        else if (option == "nodes")
        {
            // User specified number of nodes
            string nodesInput;
            parser >> nodesInput;
            nodesCount = stoi(nodesInput);
        }
        else if (option == "movetime")
        {
            // User specified time to search
            string moveTimeInput;
            parser >> moveTimeInput;
            moveTimeValue = stoi(moveTimeInput);
        }
    }
    if (nodesCount == 0)
    {
        // Max nodes have not been specified
        nodesCount = MAX_INT;
    }
    if (moveTimeValue == 0)
    {
        // Max time has not been specified
        moveTimeValue = MAX_INT;
    }
    if (depthValue == 0)
    {
        depthValue = 8;
    }
    if (perft)
    {
        // Perft search
        auto start = chrono::high_resolution_clock::now();
        int perft = startPerft(board, depthValue);
        auto stop = chrono::high_resolution_clock::now();
        cout << "Perft search to depth: " << depthValue << "\n"
             << "Took " << chrono::duration_cast<chrono::milliseconds>(stop - start).count() << "ms\n";
        cout << "Nodes: " << perft;
    }
    else
    {
        // Normal search
        cout << "Normal search to depth " << depthValue << "\n";
        // cout << "Nodes: " << startSearch(&board, depthValue) << "\n";
    }
}

void parseMakeMove(istringstream &parser)
{
    string input;
    parser >> input;
    Move move = stringToMove(input, board);
    board.makeMove(move);
}

void parseUndoMove(istringstream &parser)
{
    board.undoMove();
}

void parseDebug(istringstream &parser)
{
    string option;
    parser >> option;
    if (option == "print")
    {
        string input;
        parser >> input;
        if (input == "bitboard")
        {
            for (auto x : board.pieceBB)
            {
                printBitboard(&x);
            }
            printBitboard(&board.allPiecesBB);
            printBitboard(&board.colorBB[0]);
            printBitboard(&board.colorBB[8]);
        }
    }
}

void parseDisplay(istringstream &parser)
{
    board.printBoard();
}
