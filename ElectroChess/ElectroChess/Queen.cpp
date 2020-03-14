#include "Queen.h"

Queen::Queen(Board::COLOR c, COORD pos, int player) : Piece('Q', Piece::TYPE::QUEEN, c, pos, player) {}

Queen::Queen(Board::COLOR c, int player) : Piece('Q', Piece::TYPE::QUEEN, c, player) {}


void Queen::SetCurrentMovePattern(COORD pos, Board& board) {
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

	depth = 0;
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
void Queen::SetCurrentMovePattern(COORD pos, Board& board) {
	int depth = 0;
	for (short x = pos.X; x < Board::BoardDims::length; x++) {
		if (x != pos.X)
		{
			if (!board.Grid()[pos.Y][x]->IsOccupied())
				currentMovePattern.push_back({ short(x), short(pos.Y) });
			else if (board.Grid()[pos.Y][x]->IsOccupied()
				&& board.Grid()[pos.Y][x]->GetPlayerColor() != color
				&& depth == 0)
			{
				currentMovePattern.push_back({ short(x), short(pos.Y) });
				depth++;
			}
		}
	}
	depth = 0;
	for (short x = pos.X; x >= Board::BoardDims::left; x--) {
		if (x != pos.X)
		{
			if (!board.Grid()[pos.Y][x]->IsOccupied())
				currentMovePattern.push_back({ short(x), short(pos.Y) });
			else if (board.Grid()[pos.Y][x]->IsOccupied()
				&& board.Grid()[pos.Y][x]->GetPlayerColor() != color
				&& depth == 0)
			{
				currentMovePattern.push_back({ short(x), short(pos.Y) });
				depth++;
			}
		}
	}
	depth = 0;
	for (short y = pos.Y; y < Board::BoardDims::length; y++) {
		if (y != pos.Y)
		{
			if (!board.Grid()[y][pos.X]->IsOccupied())
				currentMovePattern.push_back({ short(pos.X), short(y) });
			else if (board.Grid()[y][pos.X]->IsOccupied()
				&& board.Grid()[y][pos.X]->GetPlayerColor() != color
				&& depth == 0)
			{
				currentMovePattern.push_back({ short(pos.X), short(y) });
				depth++;
			}
		}
	}
	depth = 0;
	for (short y = pos.Y; y >= Board::BoardDims::top; y--) {
		if (y != pos.Y)
		{
			if (!board.Grid()[y][pos.X]->IsOccupied())
				currentMovePattern.push_back({ short(pos.X), short(y) });
			else if (board.Grid()[y][pos.X]->IsOccupied()
				&& board.Grid()[y][pos.X]->GetPlayerColor() != color
				&& depth == 0)
			{
				currentMovePattern.push_back({ short(pos.X), short(y) });
				depth++;
			}
		}
	}
	depth = 0;
	for (short x = pos.X, y = pos.Y; x < Board::BoardDims::length && y < Board::BoardDims::length; x++, y++) {
		if (x != pos.X && y != pos.Y)
		{
			if (!board.Grid()[y][x]->IsOccupied())
				currentMovePattern.push_back({ short(x), short(y) });
			else if (board.Grid()[y][x]->IsOccupied()
				&& board.Grid()[y][x]->GetPlayerColor() != color
				&& depth == 0)
			{
				currentMovePattern.push_back({ short(x), short(y) });
				depth++;
			}
		}
	}
	depth = 0;
	for (short x = pos.X, y = pos.Y; x >= Board::BoardDims::left && y >= Board::BoardDims::top; x--, y--) {
		if (x != pos.X && y != pos.Y)
		{
			if (!board.Grid()[y][x]->IsOccupied())
				currentMovePattern.push_back({ short(x), short(y) });
			else if (board.Grid()[y][x]->IsOccupied()
				&& board.Grid()[y][x]->GetPlayerColor() != color
				&& depth == 0)
			{
				currentMovePattern.push_back({ short(x), short(y) });
				depth++;
			}
		}
	}
	depth = 0;
	for (short x = pos.X, y = pos.Y; x >= Board::BoardDims::left && y < Board::BoardDims::length; x--, y++) {
		if (x != pos.X && y != pos.Y)
		{
			if (!board.Grid()[y][x]->IsOccupied())
				currentMovePattern.push_back({ short(x), short(y) });
			else if (board.Grid()[y][x]->IsOccupied()
				&& board.Grid()[y][x]->GetPlayerColor() != color
				&& depth == 0)
			{
				currentMovePattern.push_back({ short(x), short(y) });
				depth++;
			}
		}
	}
	depth = 0;
	for (short x = pos.X, y = pos.Y; x < Board::BoardDims::length && y >= Board::BoardDims::top; x++, y--) {
		if (x != pos.X && y != pos.Y)
		{
			if (!board.Grid()[y][x]->IsOccupied())
				currentMovePattern.push_back({ short(x), short(y) });
			else if (board.Grid()[y][x]->IsOccupied()
				&& board.Grid()[y][x]->GetPlayerColor() != color
				&& depth == 0)
			{
				currentMovePattern.push_back({ short(x), short(y) });
				depth++;
			}
		}
	}
}
*/