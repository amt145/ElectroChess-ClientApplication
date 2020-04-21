#pragma once
#include "ElectroWin.h"
#include "Piece.h"

/*
	The player class allows the tracking of player-specific states of the game, such as
	the event of a move. It provides a collection of functions that act as an interface to
	the Game class.
*/

class Player
{
public:

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

	// Rudimentary getters and setters
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
	// ID of player, which determines which player is assigned which piece color,
	// which also determines which player goes first
	int id;
	
	// Name of the player (not really needed, but could be useful for logging)
	std::string name;

	// List of player's pieces in play
	std::vector<Piece*> pieces;

	// List of oppenent's pieces that the player has captured. 
	// It is not really needed, but although the player playing on
	// the client application can instantiate a maximum number of pieces of
	// a certain type, the physical player cannot, so it may be useful to
	// be thoughtful of what physical pieces can be used to promote other pieces
	std::vector<Piece*> captured;

	// Determines if it is currently the player's turn
	bool turn;

	// Player's piece color
	Board::COLOR color;

	// End game condition
	bool winner;

	// Since only the king can be in check, it is more useful to
	// have the check condition an attribute of the player.
	// ie it is similar to say that "the player is in check" as it
	// is to say that "the player's king is in check"
	bool check;

	// Since a finite number of players can play at one time,
	// the player count is declared as static and is used to
	// assign player IDs
	static int playerCount;
};