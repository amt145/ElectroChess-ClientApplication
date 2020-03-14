#pragma once
#include "Piece.h"

class Bishop : public Piece
{
public:
	Bishop(Board::COLOR c, COORD pos, int player);
	Bishop(Board::COLOR c, int player);
	std::vector<COORD>& GetCurrentMovePattern() { return currentMovePattern; };

	void SetCurrentMovePattern(COORD pos, Board& board);
};