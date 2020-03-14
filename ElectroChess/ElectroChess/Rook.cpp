#include "Rook.h"

Rook::Rook(Board::COLOR c, COORD pos, int player) : Piece('R', Piece::TYPE::ROOK, c, pos, player) {}
Rook::Rook(Board::COLOR c, int player) : Piece('R', Piece::TYPE::ROOK, c, player) {}

void Rook::SetCurrentMovePattern(COORD pos, Board& board) {
	currentMovePattern.clear();
	int depth = 0;
	for (short x = pos.X; x < Board::BoardDims::length; x++) {
		if (x != pos.X && !board.Grid()[pos.Y][x]->IsOccupied())
			currentMovePattern.push_back({ short(x), short(pos.Y) });
		else if (x != pos.X && board.Grid()[pos.Y][x]->IsOccupied()
			&& board.Grid()[pos.Y][x]->GetPlayerColor() != color
			&& depth == 0)
		{
			currentMovePattern.push_back({ short(x), short(pos.Y) });
			depth++;
		}
		else if (x != pos.X && board.Grid()[pos.Y][x]->IsOccupied() && board.Grid()[pos.Y][x]->GetPlayerColor() == color)
			break;
	}
	depth = 0;
	for (short x = pos.X; x >= Board::BoardDims::left; x--) {
		if (x != pos.X && !board.Grid()[pos.Y][x]->IsOccupied())
			currentMovePattern.push_back({ short(x), short(pos.Y) });
		else if (x != pos.X && board.Grid()[pos.Y][x]->IsOccupied()
			&& board.Grid()[pos.Y][x]->GetPlayerColor() != color
			&& depth == 0)
		{
			currentMovePattern.push_back({ short(x), short(pos.Y) });
			depth++;
		}
		else if (x != pos.X && board.Grid()[pos.Y][x]->IsOccupied() && board.Grid()[pos.Y][x]->GetPlayerColor() == color)
			break;
	}
	depth = 0;
	for (short y = pos.Y; y < Board::BoardDims::length; y++) {
		if (y != pos.Y && !board.Grid()[y][pos.X]->IsOccupied())
			currentMovePattern.push_back({ short(pos.X), short(y) });
		else if (y != pos.Y && board.Grid()[y][pos.X]->IsOccupied()
			&& board.Grid()[y][pos.X]->GetPlayerColor() != color
			&& depth == 0)
		{
			currentMovePattern.push_back({ short(pos.X), short(y) });
			depth++;
		}
		else if (y != pos.Y && board.Grid()[y][pos.X]->IsOccupied() && board.Grid()[y][pos.X]->GetPlayerColor() == color)
			break;
	}
	depth = 0;
	for (short y = pos.Y; y >= Board::BoardDims::top; y--) {
		if (y != pos.Y && !board.Grid()[y][pos.X]->IsOccupied())
			currentMovePattern.push_back({ short(pos.X), short(y) });
		else if (y != pos.Y && board.Grid()[y][pos.X]->IsOccupied()
			&& board.Grid()[y][pos.X]->GetPlayerColor() != color
			&& depth == 0)
		{
			currentMovePattern.push_back({ short(pos.X), short(y) });
			depth++;
		}
		else if (y != pos.Y && board.Grid()[y][pos.X]->IsOccupied() && board.Grid()[y][pos.X]->GetPlayerColor() == color)
			break;
	}
	currentCapturePattern = currentMovePattern;
}