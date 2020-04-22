/*
	File: Board.cpp
	Author: Alex Turner
	Last Revision Date: 04/24/2020
*/

#include "Board.h"

/*
	Board implementation file.
	In the Windows' console, the height of a particular character is twice its width,
	so the width of each space is double with respect to its height, so the board and its spaces are all square.
*/

// The default Board constructor initializes the space dimensions as 3x3 (2*3x3) characters
Board::Board()
	:
	spaceW(2 * 3),
	spaceH(3),
	boardWidth(8 * 2 * 3),
	boardHeight(8 * 3),
	boardArea({ 0, 0, 2 * 3 * 8, 3 * 8 })
{
	// Construct the grid of alternating-color spaces for the board
	bool white = true;
	for (int i = 1; i < 9; i++) {
		grid.push_back({});
		for (int j = 1; j < 9; j++) {
			if (white)
			{
				grid[i - 1].emplace_back(new Space({ short(j - 1), short(i - 1) }, 2 * 3, 3, boardWidth, boardHeight, Board::COLOR::WHITE, false, 0xff));
				white = false;
			}
			else if (!white)
			{
				grid[i - 1].emplace_back(new Space({ short(j - 1), short(i - 1) }, 2 * 3, 3, boardWidth, boardHeight, Board::COLOR::BLACK, false, 0xdb));
				white = true;
			}
		}
		// Ternary operation does not work here
		if (white)
			white = false;
		else
			white = true;
	}
};

// Creates a board of arbitrarily-sized spaces
Board::Board(short spaceWidth, short spaceHeight)
	: spaceW(2 * spaceWidth),
	spaceH(spaceHeight),
	boardWidth(2 * spaceWidth * 8),
	boardHeight(spaceHeight * 8),
	boardArea({ short(0), short(0), short(2 * spaceWidth * 8), short(spaceHeight * 8) })
{
	bool white = true;
	for (int i = 1; i < 9; i++) {
		grid.push_back({});
		for (int j = 1; j < 9; j++) {
			if (white)
			{
				grid[i - 1].emplace_back(new Space({ short(j - 1), short(i - 1) }, short(spaceW), short(spaceH), boardWidth, boardHeight, Board::COLOR::WHITE, false, 0xff));
				white = false;
			}
			else if (!white)
			{
				grid[i - 1].emplace_back(new Space({ short(j - 1), short(i - 1) }, short(spaceW), short(spaceH), boardWidth, boardHeight, Board::COLOR::BLACK, false, 0xdb));
				white = true;
			}
		}
		if (white)
			white = false;
		else
			white = true;
	}
};

// Destructor
Board::~Board() {
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			delete grid[i][j];
		}
	}
}

// Remove piece simply places a filler character of appropriate color in place of the occupying piece's character
void Board::Space::RemovePiece() {
	if (spaceColor == Board::COLOR::WHITE)
		Place((char)0xff, Board::COLOR::WHITE);
	else if (spaceColor == Board::COLOR::BLACK)
		Place((char)0xdb, Board::COLOR::BLACK);
}

// Place sets the center character of a space and occupies it if the parameter is not a filler character
void Board::Space::Place(char p, Board::COLOR c) {
	if (p != (char)0xdb && p != (char)0xff)
	{
		piece = p;
		if (c == Board::COLOR::RED || c == Board::COLOR::BLACK)
		{
			playerColor = Board::COLOR::BLACK;
			if (spaceColor == Board::COLOR::WHITE)
				pieceColor = colors[(int)Board::COLOR::RED_WHITE];
			else
				pieceColor = colors[(int)Board::COLOR::RED_BLACK];
		}
		else if (c == Board::COLOR::BLUE || c == Board::COLOR::WHITE)
		{
			playerColor = Board::COLOR::WHITE;
			if (spaceColor == Board::COLOR::WHITE)
				pieceColor = colors[(int)Board::COLOR::BLUE_WHITE];
			else
				pieceColor = colors[(int)Board::COLOR::BLUE_BLACK];
		}
		ForceOccupy();
	}
	else
	{
		piece = p;
		playerColor = Board::COLOR::NA;
		if (spaceColor == Board::COLOR::WHITE)
			pieceColor = colors[(int)Board::COLOR::WHITE];
		else if (spaceColor == Board::COLOR::BLACK)
			pieceColor = colors[(int)Board::COLOR::BLACK];
		Vacant();
	}
}

// Places all pieces in their initial positions on the board
void Board::Populate(Board::COLOR p2C, Board::COLOR p1C) {
	// Populate black pieces
	grid[0][0]->Place('R', p2C);
	grid[0][1]->Place('N', p2C);
	grid[0][2]->Place('B', p2C);
	grid[0][3]->Place('Q', p2C);
	grid[0][4]->Place('K', p2C);
	grid[0][5]->Place('B', p2C);
	grid[0][6]->Place('N', p2C);
	grid[0][7]->Place('R', p2C);
	for (int i = 0; i < 8; i++)
		grid[1][i]->Place('P', p2C);
	
	// Populate white pieces
	for (int i = 0; i < 8; i++)
		grid[6][i]->Place('P', p1C);
	grid[7][0]->Place('R', p1C);
	grid[7][1]->Place('N', p1C);
	grid[7][2]->Place('B', p1C);
	grid[7][3]->Place('Q', p1C);
	grid[7][4]->Place('K', p1C);
	grid[7][5]->Place('B', p1C);
	grid[7][6]->Place('N', p1C);
	grid[7][7]->Place('R', p1C);
}
