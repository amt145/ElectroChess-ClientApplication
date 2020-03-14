#include "Knight.h"

Knight::Knight(Board::COLOR c, COORD pos, int player) : Piece('N', Piece::TYPE::KNIGHT, c, pos, player) {}
Knight::Knight(Board::COLOR c, int player) : Piece('N', Piece::TYPE::KNIGHT, c, player) {}

void Knight::SetCurrentMovePattern(COORD pos, Board& board) {
	currentMovePattern.clear();
	short x = pos.X;
	short y = pos.Y;

	// Top-right two moves
	if (x + 1 <= Board::BoardDims::topRight.X
		&& y - 2 >= Board::BoardDims::topRight.Y
		&& board.Grid()[y - 2][x + 1]->GetPlayerColor() != color)
		currentMovePattern.push_back({ short(x + 1), short(y - 2) });
	
	if (x + 2 <= Board::BoardDims::topRight.X
		&& y - 1 >= Board::BoardDims::topRight.Y
		&& board.Grid()[y - 1][x + 2]->GetPlayerColor() != color)
		currentMovePattern.push_back({ short(x + 2), short(y - 1) });
	//
	// Bottom-right two moves
	if (x + 2 <= Board::BoardDims::bottomRight.X
		&& y + 1 <= Board::BoardDims::bottomRight.Y
		&& board.Grid()[y + 1][x + 2]->GetPlayerColor() != color)
		currentMovePattern.push_back({ short(x + 2), short(y + 1) });
	
	if (x + 1 <= Board::BoardDims::bottomRight.X
		&& y + 2 <= Board::BoardDims::bottomRight.Y
		&& board.Grid()[y + 2][x + 1]->GetPlayerColor() != color)
		currentMovePattern.push_back({ short(x + 1), short(y + 2) });
	//
	// Bottom-left two moves
	if (x - 1 >= Board::BoardDims::bottomLeft.X
		&& y + 2 <= Board::BoardDims::bottomLeft.Y
		&& board.Grid()[y + 2][x - 1]->GetPlayerColor() != color)
		currentMovePattern.push_back({ short(x - 1), short(y + 2) });
	if (x - 2 >= Board::BoardDims::bottomLeft.X
		&& y + 1 <= Board::BoardDims::bottomLeft.Y
		&& board.Grid()[y + 1][x - 2]->GetPlayerColor() != color)
		currentMovePattern.push_back({ short(x - 2), short(y + 1) });
	//
	// Top-left two moves
	if (x - 2 >= Board::BoardDims::topLeft.X
		&& y - 1 >= Board::BoardDims::topLeft.Y
		&& board.Grid()[y - 1][x - 2]->GetPlayerColor() != color)
		currentMovePattern.push_back({ short(x - 2), short(y - 1) });
	if (x - 1 >= Board::BoardDims::topLeft.X
		&& y - 2 >= Board::BoardDims::topLeft.Y
		&& board.Grid()[y - 2][x - 1]->GetPlayerColor() != color)
		currentMovePattern.push_back({ short(x - 1), short(y - 2) });
	//
	currentCapturePattern = currentMovePattern;
}