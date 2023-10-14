#ifndef MOVE_H
#define MOVE_H

#include <cstdint>
#include "Piece.h"

// Moves will be a 16 bit unsigned integer
typedef uint16_t Move;

// Move format:
// 0000 0000 0000 0000
// ECCP Prom  To  From
// ECCP: En Passant, Castle, Capture, Promotion

bool isCapture(Move move);
bool isCastle(Move move);
bool isEnPassant(Move move);
bool isPromotion(Move move);

Piece getPromotion(Move move);
int getTo(Move move);
int getFrom(Move move);
bool isEnPassantCapture(Move move);
bool isPromotion(Move move);
bool isCastle(Move move);
bool isCapture(Move move);

#endif
