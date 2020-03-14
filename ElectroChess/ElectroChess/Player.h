#pragma once
#include "ElectroWin.h"

#include "Piece.h"


class Player
{
public:
	/*
	Player(std::string playerName, int playerID)
		:
		name(playerName),
		id(playerID),
		turn(false) { 
		playerCount++;
		InitPieces();
	};
	*/

	Player(std::string playerName, int playerID, Board::COLOR c)
		:
		name(playerName),
		id(playerID),
		color(c),
		turn(false),
		winner(false),
		check(false)
	{ playerCount++; InitPieces(); };
	
	~Player();

	bool IsWinner() { return winner; };
	int GetID() const { return id; };
	std::string GetName() const { return name; };
	std::vector<Piece*>& GetPieceList() { return pieces; };
	Board::COLOR GetColor() const { return color; };
	std::vector<Piece*>& GetCapturedPieces() { return captured; };
	bool IsTurn() { return turn; };
	void SetName(std::string playerName) { name = playerName; };
	void SetID(int ID) { id = ID; };
	void SetTurn(bool t) { turn = t; };
	void AddPiece(Piece* piece) { pieces.push_back(piece); };
	void RemovePiece(COORD pos);
	void RemovePiece(Piece* piece);
	void SetColor(Board::COLOR c) { color = c; };
	void AddCapturedPiece(Piece* p) { captured.push_back(p); };
	void RemoveCapturedPiece(Piece* p);
	void SetPieceColor(Board::COLOR c) { color = c; };
	void InitPieces();
	void Winner() { winner = true; };
	bool IsCheck();
private:
	int id;
	//int numPieces;
	std::string name;
	std::vector<Piece*> pieces;
	std::vector<Piece*> captured;
	bool turn;
	//bool precedent;
	Board::COLOR color;
	bool winner;
	bool check;
	static int playerCount;
};