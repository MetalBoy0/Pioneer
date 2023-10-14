// Handles functions between the user and the engine relating to the UCI protocol

#include <iostream>
#include <sstream>
#include "uci.h"

#define MAX_INT ~0u;
#define MAX_DEPTH 8;

using namespace std;

Board board;

void setup()
{
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
}

void parseDebug(istringstream &parser)
{
}

void parseDisplay(istringstream &parser)
{
}
