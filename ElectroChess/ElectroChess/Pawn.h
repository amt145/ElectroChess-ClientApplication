#pragma once
#include "Piece.h"

class Pawn : public Piece
{
public:
	Pawn(Board::COLOR c, COORD pos, int player);
	Pawn(Board::COLOR c, int player);
	//~Pawn();
	std::vector<COORD>& GetCurrentMovePattern() { return currentMovePattern; };
	std::vector<COORD>& GetCurrentCapturePattern() { return currentCapturePattern; };
	long& MoveCounter() { return moveCounter; };
	void SetCurrentMovePattern(COORD pos, Board& board);
	void SetCurrentMovePattern(COORD pos, Board& board, const Pawn& other);
	void SetCurrentCapturePattern(COORD pos, Board& board);
	void SetCurrentCapturePattern(COORD pos, Board& board, const Pawn& other);
	void SetCurrentCapturePattern(COORD pos, Board& board, bool epL, bool epR);
	void MoveTo(COORD space) { pos = space; moveCounter++; if(!hasMoved) hasMoved = true; };
};