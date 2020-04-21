#pragma once
#include "ElectroWin.h"
#include "Board.h"

/*
	The Piece class serves as a base class for all derived piece classes that represent all different pieces available in chess.
*/

class Piece
{
public:
	enum class COLOR
	{
		WHITE, BLACK
	};
	enum class TYPE
	{
		PAWN,
		KING,
		BISHOP,
		KNIGHT,
		ROOK,
		QUEEN
	};
public:
	Piece() : color(Board::COLOR::WHITE), type(TYPE::PAWN), symbol('P'), pos({ 0,0 }), alive(true), ownerID(0), hasMoved(false), moveCounter(0) {};
	Piece(char sym, COORD p);
	Piece(char sym, Board::COLOR c, COORD p, int player);
	Piece(char sym, TYPE t, Board::COLOR c, COORD p, int player) : symbol(sym), type(t), color(c), pos(p), ownerID(player), alive(true), hasMoved(false), moveCounter(0) {};
	Piece(char sym, TYPE t, Board::COLOR c, int player, bool status) : symbol(sym), type(t), color(c), ownerID(player), alive(status), hasMoved(false), moveCounter(0) {};
	Piece(char sym, TYPE t, Board::COLOR c, int player) : symbol(sym), type(t), color(c), ownerID(player), alive(false), hasMoved(false), moveCounter(0) {};
	Piece(char sym, TYPE t, Board::COLOR c, int player, long moveC) : symbol(sym), type(t), color(c), ownerID(player), alive(false), hasMoved(false), moveCounter(moveC) {};

	Piece(const Piece& rhs);
	~Piece() = default;
	//virtual ~Piece() {};

	Piece& operator=(const Piece& rhs);


	virtual void MoveTo(COORD space) { pos = space; if (!hasMoved) hasMoved = true; moveCounter++; };

	// Each piece has a unique movement pattern and a unique capture pattern, therefore
	// the functions that deal with the patterns are virtual
	virtual void SetCurrentMovePattern(COORD pos, Board& board) {};
	virtual void SetCurrentCapturePattern(COORD pos, Board& board) { currentCapturePattern = currentMovePattern; };

	// Generic Piece getters and setters
	Board::COLOR GetColor() const { return color; };
	TYPE GetType() const { return type; };
	char GetSymbol() const { return symbol; };
	COORD GetPosition() const { return pos; };
	bool GetState() const { return alive; };
	virtual bool HasMoved() const { return hasMoved; };
	virtual std::vector<COORD>& GetCurrentMovePattern() { return currentMovePattern; };
	virtual std::vector<COORD>& GetCurrentCapturePattern() { return currentCapturePattern; };
	virtual long& MoveCounter() { return moveCounter; };
	virtual bool IsCheck() const { return false; };
	virtual void Check() { return; };
	void SetColor(Board::COLOR c) { color = c; };
	void SetType(TYPE t) { type = t; };
	void SetSymbol(char sym) { symbol = sym; };
	void SetPosition(COORD p) { pos = { p.X, p.Y }; };
	void SetState(bool s) { alive = s; };
	void SwitchState() { alive ? false : true; };

	bool operator ==(const Piece& rhs);
	bool operator !=(const Piece& rhs);

protected:
	// Piece color
	Board::COLOR color;
	
	// Piece type
	TYPE type;
	
	// Piece character symbol
	char symbol;
	
	// 
	COORD pos;
	
	// Piece capture status
	bool alive;

	// ID of the player the piece belongs to
	int ownerID;

	// The movement pattern of a pawn changes depending on if the piece
	// itself has moved before. Further, the capture pattern changes if
	// a piece it intends on capturing has moved previously, if the piece it
	// intends on capturing is also a pawn. The possibility of these two states
	// then must be monitored
	bool hasMoved;
	long moveCounter;

	// The move pattern is a list of coordinates in square space on the board to where a piece can move
	// based on its current position
	std::vector<COORD> currentMovePattern;
	// the capture pattern is a list of coordinates in square space on the board to where a piece can
	// move to during a capture based on its current position
	std::vector<COORD> currentCapturePattern;
};