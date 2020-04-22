/*
	File: King.h
	Author: Alex Turner
	Last Revision Date: 04/24/2020
*/

#pragma once
#include "Piece.h"

class King : public Piece
{
public:
	King(Board::COLOR c, COORD pos, int player);
	King(Board::COLOR c, COORD pos, int player, bool status);
	std::vector<COORD>& GetCurrentMovePattern() { return currentMovePattern; };
	bool IsCheck() const { return check; };

	void SetCurrentMovePattern(COORD pos, Board& board);
	void SetCheck(bool b) { check = b; };
	void Check() { if (!check) check = true; };

private:
	bool check;
};