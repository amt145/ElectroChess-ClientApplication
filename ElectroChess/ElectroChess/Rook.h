/*
	File: Rook.h
	Author: Alex Turner
	Last Revision Date: 04/24/2020
*/

#pragma once
#include "Piece.h"

class Rook : public Piece
{
public:
	Rook(Board::COLOR c, COORD pos, int player);
	Rook(Board::COLOR c, int player);
	std::vector<COORD>& GetCurrentMovePattern() { return currentMovePattern; };
	void SetCurrentMovePattern(COORD pos, Board& board);
};
