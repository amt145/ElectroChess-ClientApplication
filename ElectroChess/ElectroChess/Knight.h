#pragma once
#include "Piece.h"

class Knight : public Piece
{
public:
	Knight(Board::COLOR c, COORD pos, int player);
	Knight(Board::COLOR c, int player);

	std::vector<COORD>& GetCurrentMovePattern() { return currentMovePattern; };

	void SetCurrentMovePattern(COORD pos, Board& board);
};