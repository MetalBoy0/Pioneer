#include "board.h"
#include "bitboard.h"
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
        list->index[list->count] = index;
        list->count++;
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

void Board::clearPieceLists()
{
    for (int i = 0; i < 7; i++)
    {
        pieces[i][0].count = 0;
        pieces[i][1].count = 0;
    }
}

void Board::loadFEN(string fen, bool isWhite, bool whiteCanCastleKingSide, bool whiteCanCastleQueenSide, bool blackCanCastleKingSide, bool blackCanCastleQueenSide, int enPassantSquare)
{
    int current = 0;
    clearPieceLists();

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
            addPiece(&pieces[Pieces::getType(board[current])][Pieces::isBlack(board[current])], current);
            current++;
        }
    }
    sideToMove = isWhite ? Pieces::White : Pieces::Black;
    otherSide = isWhite ? Pieces::Black : Pieces::White;
    this->blackCanCastleKingSide = blackCanCastleKingSide;
    this->blackCanCastleQueenSide = blackCanCastleQueenSide;
    this->whiteCanCastleKingSide = whiteCanCastleKingSide;
    this->whiteCanCastleQueenSide = whiteCanCastleQueenSide;
    this->removeCastlingRightsBK = -1;
    this->removeCastlingRightsBQ = -1;
    this->removeCastlingRightsWK = -1;
    this->removeCastlingRightsWQ = -1;

    enPassantSquare = enPassantSquare;
    setupBitboards();
    attackedBB[sideToMove] = getAttackedBB(sideToMove);
    attackedBB[otherSide] = getAttackedBB(otherSide);
    inCheck = attackedBB[otherSide] & pieceBB[Pieces::King] & colorBB[sideToMove];
}
// Set's a move on the board and updates the bitboards
void Board::setMove(Move move)
{
    int from = getFrom(move);
    int to = getTo(move);

    Piece movePiece = board[from];
    Piece capturedPiece = getCapturedPiece(move);

    Pieces::Color side = Pieces::getColor(movePiece);
    Pieces::PieceType pieceType = Pieces::getType(movePiece);
    Pieces::Color capturedSide = Pieces::getColor(capturedPiece);
    Pieces::PieceType capturedType = Pieces::getType(capturedPiece);

    // Update array
    board[from] = Pieces::Empty;
    board[to] = movePiece;

    // update PieceList
    movePieceList(&pieces[pieceType][side / 8], from, to);

    // Remove piece from 'from' square
    // We need to first clear the piece from Color BB and Piece BB
    clearBit(&colorBB[side], from);
    clearBit(&pieceBB[pieceType], from);
    setBit(&pieceBB[Pieces::Empty], from);

    // Clear the captured piece
    clearBit(&pieceBB[capturedType], to);
    if (capturedPiece != Pieces::Empty)
    {
        // Update PieceList
        removePiece(&pieces[capturedType][capturedSide / 8], to);
        clearBit(&colorBB[capturedSide], to);
    }

    // Set it to the 'to' square
    // We need to first set the bit to colorBB and Piece BB
    setBit(&colorBB[side], to);
    setBit(&pieceBB[pieceType], to);
}

void Board::revertSetMove(Move move)
{
    int from = getFrom(move);
    int to = getTo(move);
    Piece movePiece = board[to];
    Piece capturedPiece = getCapturedPiece(move);

    Pieces::Color side = Pieces::getColor(movePiece);
    Pieces::PieceType pieceType = Pieces::getType(movePiece);
    Pieces::Color capturedSide = Pieces::getColor(capturedPiece);
    Pieces::PieceType capturedType = Pieces::getType(capturedPiece);

    // Update array
    board[from] = movePiece;
    board[to] = capturedPiece;

    // update PieceList
    movePieceList(&pieces[pieceType][side / 8], to, from);

    // Add piece to 'from' square
    // We need to first set the piece to Color BB and Piece BB

    clearBit(&colorBB[side], to);
    clearBit(&pieceBB[pieceType], to);

    // Set the captured piece
    setBit(&pieceBB[capturedType], to);
    if (capturedPiece != Pieces::Empty)
    {
        // Update PieceList
        addPiece(&pieces[capturedType][capturedSide / 8], to);
        setBit(&colorBB[capturedSide], to);
    }

    // Clear it from the 'to' square
    // We need to first clear the bit from colorBB and Piece BB

    setBit(&colorBB[side], from);
    setBit(&pieceBB[pieceType], from);
    clearBit(&pieceBB[Pieces::Empty], from);
}

Bitboard Board::getPieceBB(Piece piece)
{
    return pieceBB[Pieces::getType(piece)] & colorBB[Pieces::getColor(piece)];
}

Bitboard Board::getAttackedBB(Pieces::Color side)
{
    Bitboard attackedBB = 0;

    Bitboard pawnBB = pieceBB[Pieces::Pawn] & colorBB[side];
    if (side == Pieces::White)
    {
        Bitboard pawnsBB_ = pawnBB & ~fileMasks[7];
        attackedBB |= shift(&pawnsBB_, SE, 1);
        ;
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

    Bitboard noKings = allPiecesBB & ~(pieceBB[Pieces::King] & colorBB[Pieces::invertColor(side)]);

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
    int kingSquare = getLSB(&kingBB);
    attackedBB |= getAttackBB<Pieces::King>(kingSquare);

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

    if (isPromotion(move))
    {
        // Make the premoted piece a pawn
        int to = getTo(move);
        int from = getFrom(move);
        Piece promotion = getPromotion(move) | sideToMove;
        board[from] = promotion;
        setBit(&pieceBB[getPromotion(move)], from);
        clearBit(&pieceBB[Pieces::Pawn], from);
        // Update PieceList
        removePiece(&pieces[Pieces::Pawn][sideToMove / 8], from);
        addPiece(&pieces[getPromotion(move)][sideToMove / 8], from);
    }

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
    if (Pieces::isKing(board[getFrom(move)]) && (isWhite ? (whiteCanCastleKingSide | whiteCanCastleQueenSide) : (blackCanCastleKingSide | blackCanCastleQueenSide)))
    {
        if (sideToMove == Pieces::White)
        {
            whiteCanCastleKingSide = false;
            whiteCanCastleQueenSide = false;
            if (removeCastlingRightsWK == -1)
            {
                removeCastlingRightsWK = ply;
            }
            if (removeCastlingRightsWQ == -1)
            {
                removeCastlingRightsWQ = ply;
            }
        }
        else
        {
            blackCanCastleKingSide = false;
            blackCanCastleQueenSide = false;
            if (removeCastlingRightsBK == -1)
            {
                removeCastlingRightsBK = ply;
            }
            if (removeCastlingRightsBQ == -1)
            {
                removeCastlingRightsBQ = ply;
            }
        }
    }
    // Save the move to the history

    pastMoves[ply] = move;

    // Update the board

    // if enPassant
    if (getFrom(move) == 0 && board[getFrom(move)] == (Pieces::Black | Pieces::Rook) && blackCanCastleQueenSide)
    {
        blackCanCastleQueenSide = false;
        removeCastlingRightsBQ = ply;
    }
    if (getFrom(move) == 7 && board[getFrom(move)] == (Pieces::Black | Pieces::Rook) && blackCanCastleKingSide)
    {
        blackCanCastleKingSide = false;
        removeCastlingRightsBK = ply;
    }
    if (getFrom(move) == 56 && board[getFrom(move)] == (Pieces::White | Pieces::Rook) && whiteCanCastleQueenSide)
    {
        whiteCanCastleQueenSide = false;
        removeCastlingRightsWQ = ply;
    }
    if (getFrom(move) == 63 && board[getFrom(move)] == (Pieces::White | Pieces::Rook) && whiteCanCastleKingSide)
    {
        whiteCanCastleKingSide = false;
        removeCastlingRightsWK = ply;
    }
    if (getTo(move) == 0 && board[getTo(move)] == (Pieces::Black | Pieces::Rook) && blackCanCastleQueenSide)
    {
        blackCanCastleQueenSide = false;
        removeCastlingRightsBQ = ply;
    }
    if (getTo(move) == 7 && board[getTo(move)] == (Pieces::Black | Pieces::Rook) && blackCanCastleKingSide)
    {
        blackCanCastleKingSide = false;
        removeCastlingRightsBK = ply;
    }
    if (getTo(move) == 56 && board[getTo(move)] == (Pieces::White | Pieces::Rook) && whiteCanCastleQueenSide)
    {
        whiteCanCastleQueenSide = false;
        removeCastlingRightsWQ = ply;
    }
    if (getTo(move) == 63 && board[getTo(move)] == (Pieces::White | Pieces::Rook) && whiteCanCastleKingSide)
    {
        whiteCanCastleKingSide = false;
        removeCastlingRightsWK = ply;
    }

    // Update castling rights
    if (isCastle(move))
    {
        if (sideToMove == Pieces::White)
        {
            whiteCanCastleKingSide = false;
            whiteCanCastleQueenSide = false;
            if (removeCastlingRightsWK == -1)
            {
                removeCastlingRightsWK = ply;
            }
            if (removeCastlingRightsWQ == -1)
            {
                removeCastlingRightsWQ = ply;
            }
        }
        else
        {

            blackCanCastleKingSide = false;
            blackCanCastleQueenSide = false;
            if (removeCastlingRightsBK == -1)
            {
                removeCastlingRightsBK = ply;
            }
            if (removeCastlingRightsBQ == -1)
            {
                removeCastlingRightsBQ = ply;
            }
        }

        if (getTo(move) == 62)
        {
            setMove(getMove(60, 62));
            setMove(getMove(63, 61));
        }
        if (getTo(move) == 58)
        {
            setMove(getMove(60, 58));
            setMove(getMove(56, 59));
        }
        if (getTo(move) == 6)
        {
            setMove(getMove(4, 6));
            setMove(getMove(7, 5));
        }
        if (getTo(move) == 2)
        {
            setMove(getMove(4, 2));
            setMove(getMove(0, 3));
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

    inCheck = attackedBB[otherSide] & pieceBB[Pieces::King] & colorBB[sideToMove];
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
    if (isPromotion(pastMoves[ply]))
    {
        // Make the premoted piece a pawn
        int to = getTo(pastMoves[ply]);
        int from = getFrom(pastMoves[ply]);
        Piece promotion = getPromotion(pastMoves[ply]) | sideToMove;
        board[to] = Pieces::Pawn | sideToMove;
        clearBit(&pieceBB[getPromotion(pastMoves[ply])], to);
        setBit(&pieceBB[Pieces::Pawn], to);
        // Update PieceList
        removePiece(&pieces[getPromotion(pastMoves[ply])][sideToMove / 8], to);
        addPiece(&pieces[Pieces::Pawn][sideToMove / 8], to);
    }
    // Update castling rights
    if (isCastle(pastMoves[ply]))
    {
        if (getTo(pastMoves[ply]) == 62)
        {
            revertSetMove(getMove(60, 62, Pieces::Empty, true));
            revertSetMove(getMove(63, 61, Pieces::Empty, true));
        }
        if (getTo(pastMoves[ply]) == 58)
        {
            revertSetMove(getMove(60, 58, Pieces::Empty, true));
            revertSetMove(getMove(56, 59, Pieces::Empty, true));
        }
        if (getTo(pastMoves[ply]) == 6)
        {
            revertSetMove(getMove(4, 6, Pieces::Empty, true));
            revertSetMove(getMove(7, 5, Pieces::Empty, true));
        }
        if (getTo(pastMoves[ply]) == 2)
        {
            revertSetMove(getMove(4, 2, Pieces::Empty, true));
            revertSetMove(getMove(0, 3, Pieces::Empty, true));
        }
    }
    else
    {
        revertSetMove(pastMoves[ply]);
    }
    if (enPassantSquare == getTo(pastMoves[ply]))
    {
        int enemyPawn = getTo(pastMoves[ply]) + (isWhite ? 8 : -8);

        setBit(&pieceBB[Pieces::Pawn], enemyPawn);
        setBit(&colorBB[otherSide], enemyPawn);
        clearBit(&pieceBB[Pieces::Empty], enemyPawn);
        board[enemyPawn] = otherSide | Pieces::Pawn;
    }
    if (ply == removeCastlingRightsWK)
    {
        whiteCanCastleKingSide = true;
        removeCastlingRightsWK = -1;
    }
    if (ply == removeCastlingRightsBK)
    {
        blackCanCastleKingSide = true;
        removeCastlingRightsBK = -1;
    }
    if (ply == removeCastlingRightsWQ)
    {
        whiteCanCastleQueenSide = true;
        removeCastlingRightsWQ = -1;
    }
    if (ply == removeCastlingRightsBQ)
    {
        blackCanCastleQueenSide = true;
        removeCastlingRightsBQ = -1;
    }
    // Update the board

    allPiecesBB = colorBB[0] | colorBB[8]; // update all pieces bitboard

    // Update checking bitboard

    attackedBB[sideToMove] = getAttackedBB(sideToMove);
    attackedBB[otherSide] = getAttackedBB(otherSide);

    inCheck = attackedBB[otherSide] & pieceBB[Pieces::King] & colorBB[sideToMove];
}

bool Board::isCheck(Move move)
{
    makeMove(move);
    bool check = inCheck;
    undoMove();
    return check;
}

Direction Board::isPinned(int square)
{
    // Get king square
    Bitboard kingBB = colorBB[sideToMove] & pieceBB[Pieces::King];
    assert(kingBB != 0);
    int kingSquare = getLSB(&kingBB);

    // Get all enemy pieces
    Bitboard enemyPieces = colorBB[otherSide];

    Direction dir = getDirectionBetween(kingSquare, square);
    if (dir != Direction::NULLDIR && dir != Direction::NNW && dir != Direction::NNE &&
        dir != Direction::SSE && dir != Direction::SSW && dir != Direction::SEE &&
        dir != Direction::SWW && dir != Direction::NEE && dir != Direction::NWW)
    {
        Bitboard checkBB = sendRay(&allPiecesBB, dir, kingSquare);
        // If there is not a piece in between the king and the square
        if (checkBB & getBitboardFromSquare(square))
        {
            // If direction is diagonal
            if (dir == NW || dir == NE || dir == SW || dir == SE)
            {
                Bitboard attackers = sendRay(&allPiecesBB, dir, square);
                if (attackers & ((pieceBB[Pieces::Bishop] | pieceBB[Pieces::Queen]) & enemyPieces))
                {
                    return dir;
                }
            }
            if (dir == N || dir == E || dir == S || dir == W)
            {
                Bitboard attackers = sendRay(&allPiecesBB, dir, square);
                if (attackers & ((pieceBB[Pieces::Rook] | pieceBB[Pieces::Queen]) & enemyPieces))
                {
                    return dir;
                }
            }
        }
    }
    return Direction::NULLDIR;
}

bool Board::isAttacked(int square, Pieces::Color side)
{
    return getBitboardFromSquare(square) & attackedBB[side];
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
    move |= (promotion > 0) << 17;
    // Set isCapture
    move |= (isCastle ? 0 : (board[to] != 0)) << 18;
    // Set isCastle
    move |= isCastle << 19;
    // Set captured piece
    move |= (isCastle ? 0 : board[to] & 0xF) << 20;
    return move;
}

bool Board::isEnPassant(Move move)
{
    return getTo(move) == enPassantSquare && Pieces::getType(board[getFrom(move)]) == Pieces::Pawn;
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
    clearPieceLists();
    enPassantSquare = -1;
    isWhite = Pieces::isWhite(sideToMove);
    loadFEN(startFen, isWhite, whiteCanCastleKingSide, whiteCanCastleQueenSide, blackCanCastleKingSide, blackCanCastleQueenSide, enPassantSquare);
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