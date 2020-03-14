#pragma once
#include "ElectroWin.h"
#include "Board.h"

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
	virtual void SetCurrentMovePattern(COORD pos, Board& board) {};
	virtual void SetCurrentCapturePattern(COORD pos, Board& board) { currentCapturePattern = currentMovePattern; };

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
	Board::COLOR color;
	TYPE type;
	char symbol;
	COORD pos;
	bool alive;
	int ownerID;
	bool hasMoved;
	long moveCounter;
	std::vector<COORD> currentMovePattern;
	std::vector<COORD> currentCapturePattern;
};