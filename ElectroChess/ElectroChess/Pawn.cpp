/*
	File: Pawn.cpp
	Author: Alex Turner
	Last Revision Date: 04/24/2020
*/

#include "Pawn.h"

Pawn::Pawn(Board::COLOR c, COORD pos, int player) : Piece('P', Piece::TYPE::PAWN, c, pos, player) {}
Pawn::Pawn(Board::COLOR c, int player) : Piece('P', Piece::TYPE::PAWN, c, player) {}

//Pawn::~Pawn() {}

void Pawn::SetCurrentMovePattern(COORD pos, Board& board) {
	currentMovePattern.clear();
	short x = pos.X;
	short y = pos.Y;
	// Since pawns can only move one way, the pattern changes depending on who's it is
	if (ownerID == 0)
	{
		bool o = board.Grid()[y-1][x]->IsOccupied();
		//COORD pos = board.Grid()[y-1][x]->GetBoardPosition();
		if (moveCounter == 0
			&& y - 2 >= Board::BoardDims::top
			&& y - 1 >= Board::BoardDims::top
			&& !board.Grid()[y - 1][x]->IsOccupied()
			&& !board.Grid()[y - 2][x]->IsOccupied())
		{
			currentMovePattern.push_back({ pos.X, short(y - 1) });
			currentMovePattern.push_back({ pos.X, short(y - 2) });
		}
		else
		{
			if (y - 1 >= Board::BoardDims::top
				&& !board.Grid()[y - 1][x]->IsOccupied())
			{
				currentMovePattern.push_back({ pos.X, short(y - 1) });
			}
		}
	}
	else
	{
		if (moveCounter == 0
			&& y + 2 <= Board::BoardDims::bottom
			&& y + 1 <= Board::BoardDims::bottom
			&& !board.Grid()[y + 1][x]->IsOccupied()
			&& !board.Grid()[y + 2][x]->IsOccupied())
		{
			currentMovePattern.push_back({ pos.X, short(y + 1) });
			currentMovePattern.push_back({ pos.X, short(y + 2) });
		}
		else
		{
			if (y + 1 <= Board::BoardDims::bottom
				&& !board.Grid()[y + 1][x]->IsOccupied())
			{
				currentMovePattern.push_back({ pos.X, short(y + 1) });
			}
		}
	}
}

void Pawn::SetCurrentMovePattern(COORD pos, Board& board, const Pawn& other) {
	short x = pos.X;
	short y = pos.Y;
	// Since pawns can only move one way, the pattern changes depending on who's it is
	if (ownerID == 0)
	{
		bool o = board.Grid()[y - 1][x]->IsOccupied();
		COORD pos = board.Grid()[y - 1][x]->GetBoardPosition();
		if (moveCounter == 0
			&& y - 2 >= Board::BoardDims::top
			&& y - 1 >= Board::BoardDims::top
			&& !board.Grid()[y - 1][x]->IsOccupied()
			&& !board.Grid()[y - 2][x]->IsOccupied())
		{
			currentMovePattern.push_back({ pos.X, short(y - 1) });
			currentMovePattern.push_back({ pos.X, short(y - 2) });
		}
		else
		{
			if (y - 1 >= Board::BoardDims::top
				&& !board.Grid()[y - 1][x]->IsOccupied())
			{
				currentMovePattern.push_back({ pos.X, short(y - 1) });
			}
		}
	}
	else
	{
		if (moveCounter == 0
			&& y + 2 <= Board::BoardDims::bottom
			&& y + 1 <= Board::BoardDims::bottom
			&& !board.Grid()[y + 1][x]->IsOccupied()
			&& !board.Grid()[y + 2][x]->IsOccupied())
		{
			currentMovePattern.push_back({ pos.X, short(y + 1) });
			currentMovePattern.push_back({ pos.X, short(y + 2) });
		}
		else
		{
			if (y + 1 <= Board::BoardDims::bottom
				&& !board.Grid()[y + 1][x]->IsOccupied())
			{
				currentMovePattern.push_back({ pos.X, short(y + 1) });
			}
		}
	}
}

void Pawn::SetCurrentCapturePattern(COORD pos, Board& board) {
	currentCapturePattern.clear();
	short x = pos.X;
	short y = pos.Y;
	if (ownerID == 0)
	{
		if (x - 1 >= Board::BoardDims::topLeft.X
			&& y - 1 >= Board::BoardDims::topLeft.Y
			&& board.Grid()[y - 1][x - 1]->IsOccupied()
			&& board.Grid()[y - 1][x - 1]->GetPlayerColor() != color)
		{
			currentCapturePattern.push_back({ short(x - 1), short(y - 1) });
		}
		if (x + 1 <= Board::BoardDims::topRight.X
			&& y - 1 >= Board::BoardDims::topRight.Y
			&& board.Grid()[y - 1][x + 1]->IsOccupied()
			&& board.Grid()[y - 1][x + 1]->GetPlayerColor() != color)
		{
			currentCapturePattern.push_back({ short(x + 1), short(y - 1) });
		}
	}
	else
	{
		if (x + 1 <= Board::BoardDims::bottomRight.X
			&& y + 1 <= Board::BoardDims::bottomRight.Y
			&& board.Grid()[y + 1][x + 1]->IsOccupied()
			&& board.Grid()[y + 1][x + 1]->GetPlayerColor() != color)
		{
			currentCapturePattern.push_back({ short(x + 1), short(y + 1) });
		}
		if (x - 1 >= Board::BoardDims::bottomLeft.X
			&& y + 1 <= Board::BoardDims::bottomLeft.Y
			&& board.Grid()[y + 1][x - 1]->IsOccupied()
			&& board.Grid()[y + 1][x - 1]->GetPlayerColor() != color)
		{
			currentCapturePattern.push_back({ short(x - 1), short(y + 1) });
		}
	}
}

void Pawn::SetCurrentCapturePattern(COORD pos, Board& board, bool epL, bool epR) {
	short x = pos.X;
	short y = pos.Y;
	if (ownerID == 0)
	{
		if (x - 1 >= Board::BoardDims::topLeft.X
			&& y - 1 >= Board::BoardDims::topLeft.Y
			&& board.Grid()[y - 1][x - 1]->IsOccupied()
			&& board.Grid()[y - 1][x - 1]->GetPlayerColor() != color)
		{
			currentCapturePattern.push_back({ short(x - 1), short(y - 1) });
		}
		if (x + 1 <= Board::BoardDims::topRight.X
			&& y - 1 >= Board::BoardDims::topRight.Y
			&& board.Grid()[y - 1][x + 1]->IsOccupied()
			&& board.Grid()[y - 1][x + 1]->GetPlayerColor() != color)
		{
			currentCapturePattern.push_back({ short(x + 1), short(y - 1) });
		}


		if (pos.Y == short(5))
		{
			if (pos.X - short(1) >= Board::BoardDims::left
				&& board.Grid()[pos.Y][pos.X - short(1)]->IsOccupied()
				&& board.Grid()[pos.Y][pos.X - short(1)]->GetPlayerColor() != color)
			{

			}
		}

	}
	else
	{
		if (x + 1 <= Board::BoardDims::bottomRight.X
			&& y + 1 <= Board::BoardDims::bottomRight.Y
			&& board.Grid()[y + 1][x + 1]->IsOccupied()
			&& board.Grid()[y + 1][x + 1]->GetPlayerColor() != color)
		{
			currentCapturePattern.push_back({ short(x + 1), short(y + 1) });
		}
		if (x - 1 >= Board::BoardDims::bottomLeft.X
			&& y + 1 <= Board::BoardDims::bottomLeft.Y
			&& board.Grid()[y + 1][x - 1]->IsOccupied()
			&& board.Grid()[y + 1][x - 1]->GetPlayerColor() != color)
		{
			currentCapturePattern.push_back({ short(x - 1), short(y + 1) });
		}
	}
}

void Pawn::SetCurrentCapturePattern(COORD pos, Board& board, const Pawn& other) {
	short x = pos.X;
	short y = pos.Y;
	if (ownerID == 0)
	{
		if (x - 1 >= Board::BoardDims::topLeft.X
			&& y - 1 >= Board::BoardDims::topLeft.Y
			&& board.Grid()[y - 1][x - 1]->GetPlayerColor() != color)
		{
			currentCapturePattern.push_back({ short(x - 1), short(y - 1) });
		}
		if (x + 1 <= Board::BoardDims::topRight.X
			&& y - 1 >= Board::BoardDims::topRight.Y
			&& board.Grid()[y - 1][x + 1]->GetPlayerColor() != color)
		{
			currentCapturePattern.push_back({ short(x + 1), short(y - 1) });
		}
	}
	else
	{
		if (x + 1 <= Board::BoardDims::bottomRight.X
			&& y + 1 <= Board::BoardDims::bottomRight.Y
			&& board.Grid()[y + 1][x + 1]->GetPlayerColor() != color)
		{
			currentCapturePattern.push_back({ short(x + 1), short(y + 1) });
		}
		if (x - 1 >= Board::BoardDims::bottomLeft.X
			&& y + 1 <= Board::BoardDims::bottomLeft.Y
			&& board.Grid()[y + 1][x - 1]->GetPlayerColor() != color)
		{
			currentCapturePattern.push_back({ short(x - 1), short(y + 1) });
		}
	}
}