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

// appends a bitboard of pieces to a list of indexes
void appendBB(indexList *list, Bitboard BB)
{
    while (BB)
    {
        int index = popLSB(&BB);
        if (BB & 1)
        {
            list->index[list->count] = index;
            list->count++;
        }
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

Bitboard Board::getPieceBB(Piece piece)
{
    return pieceBB[Pieces::getType(piece)] & colorBB[Pieces::getColor(piece)];
}

Bitboard Board::getAttackedBB(Pieces::Color side)
{
    Bitboard attackedBB = 0;

    Bitboard pawnBB = pieceBB[Pieces::Pawn] & colorBB[side];
    if (sideToMove == Pieces::White)
    {
        Bitboard pawnsBB_ = pawnBB & ~fileMasks[7];
        attackedBB |= shift(&pawnsBB_, SE, 1);;
        pawnsBB_ = pawnBB & ~fileMasks[0];
        attackedBB |= shift(&pawnsBB_, SW, 1);
    }
    else
    {
        Bitboard pawnsBB_ = pawnBB & ~fileMasks[7];
        attackedBB |= shift(&pawnsBB_, NE, 1);
        pawnsBB_ = pawnBB & ~fileMasks[0];
        attackedBB |= shift(&pawnsBB_, NW, 1);
    }

    // Knights
    Bitboard knightBB = pieceBB[Pieces::Knight] & colorBB[side];

    while (knightBB)
    {
        attackedBB |= getAttackBB<Pieces::Knight>(popLSB(&knightBB));
    }

    // Bishops and Queens

    Bitboard noKings = allPiecesBB & ~pieceBB[Pieces::King];

    Bitboard bishopBB = (pieceBB[Pieces::Bishop] | pieceBB[Pieces::Queen]) & colorBB[side];

    while (bishopBB)
    {
        attackedBB |= getAttackBB<Pieces::Bishop>(popLSB(&bishopBB), &noKings);
    }

    // Rooks and Queens
    Bitboard rookBB = (pieceBB[Pieces::Rook] | pieceBB[Pieces::Queen]) & colorBB[side];

    while (rookBB)
    {
        attackedBB |= getAttackBB<Pieces::Rook>(popLSB(&rookBB), &noKings);
    }

    // Kings
    Bitboard kingBB = pieceBB[Pieces::King] & colorBB[side];

    for (int i = 0; i < 8; i++)
    {
        Bitboard bb = shift(&kingBB, queenDirections[i], 1) & ~colorBB[side];
        attackedBB |= bb;
    }

    return attackedBB;
}

indexList Board::piecesAttackingSquare(int square)
{
    // The idea is toz check the squares that the king can be attacked from
    // If an enemy piece is on one of those squares, the king is in check

    // Get king square
    // Note that king square means whatever square is specified, not necessarily the king
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
    pawnBB = pieceBB[Pieces::Pawn] & enemyPieces;
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

    while (bishopBB)
    {
        int bishopSquare = popLSB(&bishopBB);
        for (int i = 0; i < 4; i++)
        { // Might need to change bishopBB back to allpieceBB
            Bitboard checkBB = sendRay(&allPiecesBB, bishopDirs[i], bishopSquare);
            if (checkBB & kingBB)
            {
                checkBB = kingBB & checkBB;
                appendBB(&attackers, checkBB);
            }
        }
    }

    // Check for rooks and queens
    Direction attackingDirSliding[] = {N, E, S, W};
    Bitboard rookBB = (pieceBB[Pieces::Rook] | pieceBB[Pieces::Queen]) & enemyPieces;
    while (rookBB)
    {
        int rookSquare = popLSB(&rookBB);
        for (int i = 0; i < 4; i++)
        {
            Bitboard checkBB = sendRay(&rookBB, attackingDirSliding[i], rookSquare);
            if (checkBB & kingBB)
            {
                checkBB = kingBB & checkBB;
                appendBB(&attackers, checkBB);
            }
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
    bool moveEnPassant = isEnPassant(move);
    enPassantHistory[ply] = enPassantSquare;
    enPassantSquare = -1;
    // Update en passant square
    if (Pieces::isPawn(board[getFrom(move)]))
    {
        if (abs(getFrom(move) - getTo(move)) == 16)
        {
            if (sideToMove == Pieces::White)
            {
                enPassantSquare = getFrom(move) - 8;
            }
            else
            {
                enPassantSquare = getFrom(move) + 8;
            }
        }
    }

    // Save the move to the history

    pastMoves[ply] = move;

    // Update the board

    // if enPassant

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
    else if (moveEnPassant)
    {
        setMove(move);

        // Remove enemy pawn

        int enemyPawn = getTo(move) + (isWhite ? 8 : -8);

        board[enemyPawn] = Pieces::Empty;
        clearBit(&pieceBB[Pieces::Pawn], enemyPawn);
        clearBit(&colorBB[otherSide], enemyPawn);
        setBit(&pieceBB[Pieces::Empty], enemyPawn);
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
    allPiecesBB = colorBB[Pieces::White] | colorBB[Pieces::Black];

    // Update checking bitboard

    attackedBB[sideToMove] = getAttackedBB(sideToMove);
    attackedBB[otherSide] = getAttackedBB(otherSide);

    inCheck = getBit(&attackedBB[otherSide], __builtin_ctzll(pieceBB[Pieces::King | sideToMove]));

    if (inCheck)
    {
        
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
    if (enPassantSquare == getTo(pastMoves[ply]))
    {
        int enemyPawn = getTo(pastMoves[ply]) + (isWhite ? 8 : -8);

        setBit(&pieceBB[Pieces::Pawn], enemyPawn);
        setBit(&colorBB[otherSide], enemyPawn);
        clearBit(&pieceBB[Pieces::Empty], enemyPawn);
        board[enemyPawn] = otherSide | Pieces::Pawn;
    }

    // Update the board
    revertSetMove(pastMoves[ply]);
    allPiecesBB = colorBB[0] | colorBB[8]; // update all pieces bitboard

    // Update checking bitboard

    attackedBB[sideToMove] = getAttackedBB(sideToMove);
    attackedBB[otherSide] = getAttackedBB(otherSide);
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
    // Set captured piece
    move |= (board[to] & 0xF) << 20;
    return move;
}

bool Board::isEnPassant(Move move)
{
    return getTo(move) == enPassantSquare;
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
    enPassantSquare = -1;
    isWhite = Pieces::isWhite(sideToMove);
    loadFEN(startFen);
    initDirections();
    initBBs();
    attackedBB[sideToMove] = getAttackedBB(sideToMove);
    attackedBB[otherSide] = getAttackedBB(otherSide);
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