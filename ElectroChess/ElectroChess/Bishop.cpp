#include "Bishop.h"

Bishop::Bishop(Board::COLOR c, COORD pos, int player) : Piece('B', Piece::TYPE::BISHOP, c, pos, player) {}
Bishop::Bishop(Board::COLOR c, int player) : Piece('B', Piece::TYPE::BISHOP, c, player) {}
void Bishop::SetCurrentMovePattern(COORD pos, Board& board) {
	currentMovePattern.clear();
	int depth = 0;
	for (short x = pos.X, y = pos.Y; x < Board::BoardDims::length && y < Board::BoardDims::length; x++, y++) {
		if (x != pos.X && y != pos.Y && !board.Grid()[y][x]->IsOccupied())
			currentMovePattern.push_back({ short(x), short(y) });
		else if (x != pos.X && y != pos.Y && board.Grid()[y][x]->IsOccupied()
			&& board.Grid()[y][x]->GetPlayerColor() != color
			&& depth == 0)
		{
			currentMovePattern.push_back({ short(x), short(y) });
			depth++;
		}
		else if (x != pos.X && y != pos.Y && board.Grid()[y][x]->IsOccupied() && board.Grid()[y][x]->GetPlayerColor() == color)
			break;
	}
	depth = 0;
	for (short x = pos.X, y = pos.Y; x >= Board::BoardDims::left && y >= Board::BoardDims::top; x--, y--) {
		if (x != pos.X && y != pos.Y && !board.Grid()[y][x]->IsOccupied())
			currentMovePattern.push_back({ short(x), short(y) });
		else if (x != pos.X && y != pos.Y && board.Grid()[y][x]->IsOccupied()
			&& board.Grid()[y][x]->GetPlayerColor() != color
			&& depth == 0)
		{
			currentMovePattern.push_back({ short(x), short(y) });
			depth++;
		}
		else if (x != pos.X && y != pos.Y && board.Grid()[y][x]->IsOccupied() && board.Grid()[y][x]->GetPlayerColor() == color)
			break;
	}
	depth = 0;
	for (short x = pos.X, y = pos.Y; x >= Board::BoardDims::left && y < Board::BoardDims::length; x--, y++) {
		if (x != pos.X && y != pos.Y && !board.Grid()[y][x]->IsOccupied())
			currentMovePattern.push_back({ short(x), short(y) });
		else if (board.Grid()[y][x]->IsOccupied()
			&& board.Grid()[y][x]->GetPlayerColor() != color
			&& depth == 0)
		{
			currentMovePattern.push_back({ short(x), short(y) });
			depth++;
		}
		else if (x != pos.X && y != pos.Y && board.Grid()[y][x]->IsOccupied() && board.Grid()[y][x]->GetPlayerColor() == color)
			break;
	}
	depth = 0;
	for (short x = pos.X, y = pos.Y; x < Board::BoardDims::length && y >= Board::BoardDims::top; x++, y--) {
		if (x != pos.X && y != pos.Y && !board.Grid()[y][x]->IsOccupied())
			currentMovePattern.push_back({ short(x), short(y) });
		else if (x != pos.X && y != pos.Y && board.Grid()[y][x]->IsOccupied()
			&& board.Grid()[y][x]->GetPlayerColor() != color
			&& depth == 0)
		{
			currentMovePattern.push_back({ short(x), short(y) });
			depth++;
		}
		else if (x != pos.X && y != pos.Y && board.Grid()[y][x]->IsOccupied() && board.Grid()[y][x]->GetPlayerColor() == color)
			break;
	}
	currentCapturePattern = currentMovePattern;
}









/*
short x = pos.X;
short y = pos.Y;

// Find bottom-left-most coordinate
while (x > Board::BoardDims::bottomLeft.X && y < Board::BoardDims::bottomLeft.Y) {
	x--;
	y++;
}
// Store coordinates on the diagonal that intersects the coordinates the bishop is on
while (x < Board::BoardDims::topRight.X && y > Board::BoardDims::topRight.Y) {
	if (x != pos.X && y != pos.Y)
	{
		currentMovePattern.push_back({ short(x), short(y) });
		x++;
		y--;
	}
}
x = pos.X;
y = pos.Y;
// Find bottom-right-most coordinate
while (x < Board::BoardDims::bottomRight.X && y < Board::BoardDims::bottomRight.Y) {
	x++;
	y++;
}
while (x > Board::BoardDims::topLeft.X && y > Board::BoardDims::topLeft.Y) {
	if (x != pos.X && y != pos.Y)
	{
		currentMovePattern.push_back({ short(x), short(y) });
		x--;
		y--;
	}
}
*/