#pragma once
#include "ElectroWin.h"

/*
	The Board class handles the OOP high-level logic of the game board.
*/
class Board
{
	friend class Piece;
public:
	const static struct BoardDims // Coords in square space
	{
		static constexpr short length = 8, width = 8;
		static constexpr short left = 0;
		static constexpr short top = 0;
		static constexpr short right = 7;
		static constexpr short bottom = 7;
		static constexpr COORD topLeft = { short(0), short(0) };
		static constexpr COORD topRight = { short(7), short(0) };
		static constexpr COORD bottomLeft = { short(0), short(7) };
		static constexpr COORD bottomRight = { short(7), short(7) };
	};
	// Board space color index enumeration (FORGROUND_BACKGROUND)
	enum class COLOR
	{
		WHITE, BLACK, WHITE_BLACK, BLACK_WHITE, BLUE_WHITE, BLUE_BLACK, RED_WHITE, RED_BLACK, BLUE, RED, NA
	};
	// win32 console API-defined colors
	static constexpr WORD colors[8] = {
			FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY
				| BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY,
		
			FOREGROUND_INTENSITY | BACKGROUND_INTENSITY,
		
			FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY
				| BACKGROUND_INTENSITY,
			
			FOREGROUND_INTENSITY
				| BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY,

			FOREGROUND_BLUE
				| BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY,
			
			FOREGROUND_BLUE
				| BACKGROUND_INTENSITY,

			FOREGROUND_RED
				| BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY,

			FOREGROUND_RED
				| BACKGROUND_INTENSITY
	};
protected:
	// Embedded space class for the game board: the board populates with these structures
	class Space
	{
	public:
		Space() = delete;
		// Constructor allows game boards consisting of arbitrary number of spaces
		Space(COORD pos, short w, short h, int boardW, int boardH, Board::COLOR col, bool taken, char p)
			:
			spacePosition({ pos.X, pos.Y }),
			gridPosition({ short(pos.X * w), short(pos.Y * h) }),
			// The spot on each space where the occupying piece's character representation is always the center character in the space
			piecePosition({ gridPosition.X + short(w / 2 - 1), gridPosition.Y + short((h - 1) / 2) }),
			width(w),
			height(h),
			spaceColor(col),
			pieceColor(colors[(int)spaceColor]),
			occupied(taken),
			piece(p),
			playerColor(COLOR::NA)
		{};
		COORD GetBoardPosition() const { return spacePosition; };
		COORD GetGridPosition() const { return gridPosition; };
		COORD GetPiecePosition() const { return piecePosition; };
		short GetPiecePositionX() const { return piecePosition.X; };
		short GetPiecePositionY() const { return piecePosition.Y; };
		Board::COLOR GetSpaceColor() const { return spaceColor; };
		WORD GetPieceColor() const { return pieceColor; };
		bool IsOccupied() const { return occupied; };
		char GetPieceSymbol() const { return piece; };
		Board::COLOR GetPlayerColor() const { return playerColor; };
		Space& GetSpace() { return *this; };

		void Occupy() { if (!occupied) occupied = true; };
		void Vacant() { occupied = false; };
		void ForceOccupy() { occupied = true; };
		void Place(char p, Board::COLOR c);
		void SetPlayerColor(Board::COLOR c) { playerColor = c; };
		void RemovePiece();

		// Translate piece ID enumeration
		int GetPieceEnumeration()
		{
			switch (piece)
			{
			case 'P':
			case 'p':
				return 1;
				break;
			case 'R':
			case 'r':
				return 2;
				break;
			case 'N':
			case 'n':
				return 3;
				break;
			case 'B':
			case 'b':
				return 4;
				break;
			case 'Q':
			case 'q':
				return 5;
				break;
			case 'K':
			case 'k':
				return 6;
				break;
			default:
				return -1;
				break;
			}
		}
	private:
		// Position of space in square space
		COORD spacePosition;
		// Position of space in character space
		COORD gridPosition;
		// Position of the piece character inside the space, in square space
		COORD piecePosition;
		// Width of the space
		short width;
		// Height of the space
		short height;
		// Color of the space
		Board::COLOR spaceColor;
		// Color of the occupying piece's character
		WORD pieceColor;
		// Used to identify which player owns the occupying piece
		Board::COLOR playerColor;
		// Used to determine if a space is empty or not
		bool occupied;
		// Occupying piece's character representation
		char piece;
	};
public:
	Board();
	Board(short spaceWidth, short spaceHeight);

	~Board();

	virtual void PrintBoard() {};

	void Populate(COLOR p2C, COLOR p1C);
	const WORD* GetColorPallete() const { return colors; };
	std::vector<std::vector<Space*>>& Grid() { return grid; };


	// board width in characters
	int GetBoardWidth() const { return boardWidth; };
	
	// board height in characters
	int GetBoardHeight() const { return boardHeight; };
	
	// space width in characters
	int GetSpaceWidth() const { return spaceW; };
	
	// space height in characters
	int GetSpaceHeight() const { return spaceH; };

	SMALL_RECT GetBoardArea() const { return boardArea; };
private:
	// Space dimensions in characters
	int spaceW;
	int spaceH;
	
	// Dimensions in spaces
	int boardWidth;
	int boardHeight;

	// Rectangle representation of the board
	SMALL_RECT boardArea;

	// The handle to the board and it's populated spaces
	std::vector<std::vector<Space*>> grid;
};