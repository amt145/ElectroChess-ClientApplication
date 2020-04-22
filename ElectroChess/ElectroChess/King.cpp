/*
	File: King.cpp
	Author: Alex Turner
	Last Revision Date: 04/24/2020
*/

#include "King.h"

King::King(Board::COLOR c, COORD pos, int player) : Piece('K', Piece::TYPE::KING, c, pos, player), check(false) {}
King::King(Board::COLOR c, COORD pos, int player, bool status) : Piece('K', c, pos, player), check(status) {}

void King::SetCurrentMovePattern(COORD pos, Board& board) {
	short x = pos.X;
	short y = pos.Y;

	if (x - 1 >= Board::BoardDims::topLeft.X
		&& y - 1 >= Board::BoardDims::topLeft.Y
		&& board.Grid()[y - 1][x - 1]->GetPlayerColor() != color)
		currentMovePattern.push_back({ short(x - 1), short(y - 1) });
	
	if (y - 1 >= Board::BoardDims::top
		&& board.Grid()[y - 1][x]->GetPlayerColor() != color)
		currentMovePattern.push_back({ short(x), short(y - 1) });
	
	if (x + 1 <= Board::BoardDims::topRight.X
		&& y - 1 >= Board::BoardDims::topRight.Y
		&& board.Grid()[y - 1][x + 1]->GetPlayerColor() != color)
		currentMovePattern.push_back({ short(x + 1), short(y - 1) });

	if (x + 1 <= Board::BoardDims::right
		&& board.Grid()[y][x + 1]->GetPlayerColor() != color)
		currentMovePattern.push_back({ short(x + 1), short(y) });

	if (x + 1 <= Board::BoardDims::bottomRight.X
		&& y + 1 <= Board::BoardDims::bottomRight.Y
		&& board.Grid()[y + 1][x + 1]->GetPlayerColor() != color)
		currentMovePattern.push_back({ short(x + 1), short(y + 1) });

	if (y + 1 <= Board::BoardDims::bottom
		&& board.Grid()[y + 1][x]->GetPlayerColor() != color)
		currentMovePattern.push_back({ short(x), short(y + 1) });

	if (x - 1 >= Board::BoardDims::bottomLeft.X
		&& y + 1 <= Board::BoardDims::bottomLeft.Y
		&& board.Grid()[y + 1][x - 1]->GetPlayerColor() != color)
		currentMovePattern.push_back({ short(x - 1), short(y + 1) });

	if ( x - 1 >= Board::BoardDims::left
		&& board.Grid()[y][x - 1]->GetPlayerColor() != color)
		currentMovePattern.push_back({ short(x - 1), short(y) });
	currentCapturePattern = currentMovePattern;
}