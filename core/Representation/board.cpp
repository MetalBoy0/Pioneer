#include "board.h"
#include "../MoveGeneration/movegen.h"
#include <cassert>
// Resets bitboards based on the board array

int invertX(int i)
{
    int x = 7 - i % 8;
    int y = i / 8;
    return x + y * 8;
}

void appendBB(indexList *list, Bitboard BB)
{
    int index = 0;
    while (BB)
    {
        if (BB & 1)
        {
            list->index[list->count] = index;
            list->count++;
        }
        BB >>= 1;
        index++;
    }
}

void Board::setupBitboards()
{
    checkingBB = 0xFFFFFFFFFFFFFFFFULL;
    for (int i = 0; i < 7; i++)
    {
        pieceBB[i] = 0;
    }
    colorBB[0] = 0;
    colorBB[8] = 0;

    int current = 0;
    for (Piece x : board)
    {
        // Get piece info
        Pieces::Color side = Pieces::getColor(x);
        Pieces::PieceType type = Pieces::getType(x);

        // Set bitboards
        setBit(&pieceBB[type], current);
        setBit(&colorBB[side], current);
        current++;
    }
    allPiecesBB = colorBB[0] | colorBB[8];
}

void Board::loadFEN(string fen)
{
    int current = 0;
    for (auto x : fen)
    {
        if (x == '/')
        {
            // Skip if newline
            continue;
        }
        if (!isalpha(x))
        {
            for (int i = 0; i < x - '0'; i++)
            {
                board[current] = Pieces::Empty;
                current++;
            }
        }
        else
        {
            board[current] = Pieces::charToPiece(x);
            current++;
        }
    }
    setupBitboards();
}
// Set's a move on the board and updates the bitboards
void Board::setMove(Move move)
{
    int from = getFrom(move);
    int to = getTo(move);
    Piece movePiece = board[from];
    Piece capturedPiece = board[to];

    // Update array
    board[from] = Pieces::Empty;
    board[to] = movePiece;

    // Update bitboards
    // Remove captured piece
    // it doesn't matter if we remove an empty square because it will be updated anyway
    clearBit(&pieceBB[Pieces::getType(capturedPiece)], to);
    // It does matter if we remove a piece from the colorBB without checking if it is a capture
    if (capturedPiece != Pieces::Empty)
    {
        clearBit(&colorBB[Pieces::getColor(capturedPiece)], getTo(move));
    }
    // Add piece to new square
    setBit(&pieceBB[Pieces::getType(movePiece)], to);
    setBit(&colorBB[Pieces::getColor(movePiece)], to);

    // Remove piece from old square

    clearBit(&colorBB[Pieces::getColor(movePiece)], from);
    clearBit(&pieceBB[Pieces::getType(movePiece)], from);

    // Add old square to empty squares bitboard
    setBit(&pieceBB[Pieces::Empty], from);
}

void Board::revertSetMove(Move move)
{
    Piece movePiece = board[getTo(move)];
    Piece capturedPiece = getCapturedPiece(move);

    // Update array
    board[getFrom(move)] = movePiece;
    board[getTo(move)] = capturedPiece;

    // Update bitboards
    // Remove captured piece
    setBit(&pieceBB[Pieces::getType(capturedPiece)], getTo(move));
    if (isCapture(move))
    {
        setBit(&colorBB[Pieces::getColor(capturedPiece)], getTo(move));
    }
    // Add movepiece to original square
    setBit(&pieceBB[Pieces::getType(movePiece)], getFrom(move));
    setBit(&colorBB[Pieces::getColor(movePiece)], getFrom(move));

    // Remove movepiece from new square
    clearBit(&colorBB[Pieces::getColor(movePiece)], getTo(move));
    clearBit(&pieceBB[Pieces::getType(movePiece)], getTo(move));

    // Remove old square from empty squares bitboard
    clearBit(&pieceBB[Pieces::Empty], getFrom(move));
}

indexList Board::piecesAttackingSquare(int square)
{
    // The idea is to check the squares that the king can be attacked from
    // If an enemy piece is on one of those squares, the king is in check

    // Get king square
    indexList attackers;
    attackers.count = 0;
    Bitboard kingBB = getBitboardFromSquare(square);
    int kingSquare = getLSB(&kingBB);

    // Get all enemy pieces
    Bitboard enemyPieces = colorBB[otherSide];
    Bitboard checkBB;
    // Check for pawns
    Bitboard pawnBB = pieceBB[Pieces::Pawn] & enemyPieces;
    if (sideToMove == Pieces::White)
    {
        pawnBB = shift(&pawnBB, NE, 1);
    }
    else
    {
        pawnBB = shift(&pawnBB, SW, 1);
    }
    if (kingBB & pawnBB)
    {
        checkBB = kingBB & pawnBB;
        appendBB(&attackers, checkBB);
    }
    if (sideToMove == Pieces::White)
    {
        pawnBB = shift(&pawnBB, NW, 1);
    }
    else
    {
        pawnBB = shift(&pawnBB, SE, 1);
    }
    if (kingBB & pawnBB)
    {
        checkBB = kingBB & pawnBB;
        appendBB(&attackers, checkBB);
    }

    // Check for knights
    Bitboard knightBB = pieceBB[Pieces::Knight] & enemyPieces;
    Direction knightDirs[] = {NNE, NNW, SSE, SSW, NEE, NWW, SEE, SWW};

    for (int i = 0; i < 8; i++)
    {
        Bitboard bb = shift(&knightBB, knightDirs[i], 1);
        if (kingBB & bb)
        {
            checkBB = kingBB & bb;
            appendBB(&attackers, checkBB);
        }
    }

    // Check for bishops and queens
    Bitboard bishopBB = (pieceBB[Pieces::Bishop] | pieceBB[Pieces::Queen]) & enemyPieces;
    Direction bishopDirs[] = {NE, NW, SE, SW};

    for (int i = 0; i < 4; i++)
    { // Might need to change bishopBB back to allpieceBB
        Bitboard checkBB = sendRay(&bishopBB, bishopDirs[i], kingSquare);
        if (checkBB & bishopBB)
        {
            checkBB = kingBB & checkBB;
            appendBB(&attackers, checkBB);
        }
    }

    // Check for rooks and queens
    Direction attackingDirSliding[] = {N, E, S, W};
    Bitboard rookBB = (pieceBB[Pieces::Rook] | pieceBB[Pieces::Queen]) & enemyPieces;

    for (int i = 0; i < 4; i++)
    {
        Bitboard checkBB = sendRay(&rookBB, attackingDirSliding[i], kingSquare);
        if (checkBB & rookBB)
        {
            checkBB = kingBB & checkBB;
            appendBB(&attackers, checkBB);
        }
    }

    return attackers;
}

indexList Board::getCheckers()
{
    Bitboard kingBB = colorBB[sideToMove] & pieceBB[Pieces::King];
    return piecesAttackingSquare(getLSB(&kingBB));
}

// Plays a move on the board
void Board::makeMove(Move move)
{
    // Save the move to the history

    pastMoves[ply] = move;
    enPassantHistory[ply] = enPassantSquare;

    // Update the board

    // Update en passant square
    if (isEnPassantCapture(move))
    {
        if (sideToMove == Pieces::White)
        {
            enPassantSquare = getTo(move) - 8;
        }
        else
        {
            enPassantSquare = getTo(move) + 8;
        }
    }
    else
    {
        enPassantSquare = -1;
    }

    // Update castling rights
    if (isCastle(move))
    {
        if (sideToMove == Pieces::White)
        {
            whiteCanCastleKingSide = false;
            whiteCanCastleQueenSide = false;
        }
        else
        {
            blackCanCastleKingSide = false;
            blackCanCastleQueenSide = false;
        }
        if (getTo(move) == 62)
        {
            setMove(getMove(60, 62));
            setMove(getMove(63, 61));
        }
    }
    else
    {
        setMove(move);
    }
    // Update ply
    ply++;
    // Update side to move
    // Side = White, sideToMove = 0 * -1 + 8
    // Side = White, sideToMove = 8 | Side is now black
    // Side = Black, sideToMove = 8 * -1 + 8
    // Side = Black, sideToMove = -8+8
    // Side = Black, sideToMove = 0 | Side is now white
    sideToMove = static_cast<Pieces::Color>(sideToMove * -1 + 8); // Simple equation to flip sides of board
    otherSide = static_cast<Pieces::Color>(sideToMove * -1 + 8);
    isWhite = Pieces::isWhite(sideToMove);

    // Update allpiece bitboard
    allPiecesBB = colorBB[0] | colorBB[8];

    // Update checking bitboard
    checks = getCheckers();
    if (checks.count == 1)
    {
        Bitboard kingBB = colorBB[sideToMove] & pieceBB[Pieces::King];
        checkingBB = bitboardRay(checks.index[0], getLSB(&kingBB)) & allPiecesBB;
    }
    else
    {
        checkingBB = 0xFFFFFFFFFFFFFFFFULL;
    }
}

void Board::undoMove()
{
    // Update ply
    ply--;
    // Update side to move
    sideToMove = static_cast<Pieces::Color>(sideToMove * -1 + 8); // Simple equation to flip sides of board
    otherSide = static_cast<Pieces::Color>(sideToMove * -1 + 8);
    isWhite = Pieces::isWhite(sideToMove);

    // Update en passant square
    enPassantSquare = enPassantHistory[ply];

    // Update castling rights
    if (isCastle(pastMoves[ply]))
    {
        if (sideToMove == Pieces::White)
        {
            whiteCanCastleKingSide = true;
            whiteCanCastleQueenSide = true;
        }
        else
        {
            blackCanCastleKingSide = true;
            blackCanCastleQueenSide = true;
        }
    }

    // Update the board
    revertSetMove(pastMoves[ply]);
    allPiecesBB = colorBB[0] | colorBB[8]; // update all pieces bitboard
}

Move Board::getMove(int from, int to, Piece promotion, bool isCastle)
{
    // Move is formatted as follows:
    // 0000 0000 000000 000000
    // ECCP PROM   TO    FROM
    // ECCP: En Passant, Castle, Capture, Promotion
    Move move = 0;
    // Set from
    move |= from;
    // Set to
    move |= to << 6;
    // Set promotion
    move |= promotion << 12;
    // Set isPromote
    move |= promotion > 0 << 17;
    // Set isCapture
    move |= getBit(&allPiecesBB, to) << 18;
    // Set isCastle
    move |= isCastle << 19;
    // Set isEnPassant
    move |= (to == enPassantSquare) << 20;
    // Set captured piece
    move |= (board[to] & 0xF) << 20;
    return move;
}

Board::Board()
{
    ply = 0;
    sideToMove = Pieces::White;
    otherSide = Pieces::Black;
    blackCanCastleKingSide = true;
    blackCanCastleQueenSide = true;
    whiteCanCastleKingSide = true;
    whiteCanCastleQueenSide = true;
    isWhite = Pieces::isWhite(sideToMove);
    loadFEN(startFen);
    initDirections();
    initBBs();
    assert(~pieceBB[0] == allPiecesBB);
}

void Board::clearBoard()
{
    for (int i = 0; i < 64; i++)
    {
        board[i] = 0;
    }
}

void Board::printBoard()
{
    int index = 0;
    for (Piece p : board)
    {
        if (index % 8 == 0)
        {
            cout << endl;
        }
        cout << Pieces::pieceToChar(p) << " ";
        index++;
    }
    cout << endl;
}