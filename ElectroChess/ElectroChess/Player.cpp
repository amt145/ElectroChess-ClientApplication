#include "Player.h"

#include "Pawn.h"
#include "Rook.h"
#include "Bishop.h"
#include "Knight.h"
#include "Queen.h"
#include "King.h"

int Player::playerCount = 0;

Player::~Player() {
	for (size_t i = 0; i < pieces.size(); i++) {
		delete pieces[i];
	}
	for (size_t i = 0; i < captured.size(); i++) {
		delete captured[i];
	}
}

void Player::RemovePiece(Piece* piece) {
	std::vector<Piece*>::iterator it;
	for (it = pieces.begin();  it != pieces.end(); ++it) {
		if (*it == piece)
			break;
	}
	pieces.erase(std::remove(pieces.begin(), pieces.end(), *it), pieces.end());
}
void Player::RemovePiece(COORD pos) {
	std::vector<Piece*>::iterator it;
	for (it = pieces.begin(); it != pieces.end(); ++it) {
		if ((*it)->GetPosition().X == pos.X && (*it)->GetPosition().Y == pos.Y)
			break;
	}
	pieces.erase(std::remove(pieces.begin(), pieces.end(), *it), pieces.end());
}

void Player::RemoveCapturedPiece(Piece* piece) {
	std::vector<Piece*>::iterator it;
	for (it = captured.begin(); it != captured.end(); ++it) {
		if (*it == piece)
			break;
	}
	captured.erase(it);
}

void Player::InitPieces() {

	if (color == Board::COLOR::WHITE)
	{
		pieces.push_back(new Rook(color,   { short(0), short(7) }, id));
		pieces.push_back(new Knight(color, { short(1), short(7) }, id));
		pieces.push_back(new Bishop(color, { short(2), short(7) }, id));
		pieces.push_back(new Queen(color,  { short(3), short(7) }, id));
		pieces.push_back(new King(color,   { short(4), short(7) }, id));
		pieces.push_back(new Bishop(color, { short(5), short(7) }, id ));
		pieces.push_back(new Knight(color, { short(6), short(7) }, id));
		pieces.push_back(new Rook(color,   { short(7), short(7) }, id));
		for (short x = 0; x < 8; x++) {
			pieces.push_back(new Pawn(color, { x, short(6) }, id));
		}
	}
	else
	{
		pieces.push_back(new Rook(color,   { short(0), short(0) }, id));
		pieces.push_back(new Knight(color, { short(1), short(0) }, id));
		pieces.push_back(new Bishop(color, { short(2), short(0) }, id));
		pieces.push_back(new Queen(color,  { short(3), short(0) }, id));
		pieces.push_back(new King(color,   { short(4), short(0) }, id));
		pieces.push_back(new Bishop(color, { short(5), short(0) }, id));
		pieces.push_back(new Knight(color, { short(6), short(0) }, id));
		pieces.push_back(new Rook(color,   { short(7), short(0) }, id));
		for (short x = 0; x < 8; x++) {
			pieces.push_back(new Pawn(color, { x, short(1) }, id));
		}
	}
}

bool Player::IsCheck() {
	std::vector<Piece*>::iterator p = pieces.begin();
	for (; p != pieces.end(); ++p) {
		if ((*p)->IsCheck())
		{
			check = true;
			return true;
		}
		else
		{
			check = false;
			return false;
		}
	}
}