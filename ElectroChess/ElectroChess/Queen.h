#pragma once
#include "Piece.h"

class Queen : public Piece
{
public:
	Queen(Board::COLOR, COORD pos, int player);
	Queen(Board::COLOR c, int player);
	std::vector<COORD>& GetCurrentMovePattern() { return currentMovePattern; };

	void SetCurrentMovePattern(COORD pos, Board& board);
};