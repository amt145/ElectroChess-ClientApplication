/*
	File: Piece.cpp
	Author: Alex Turner
	Last Revision Date: 04/24/2020
*/

#include "Piece.h"

Piece::Piece(char sym, COORD p) : symbol(sym), pos({ p.X, p.Y }), alive(true), hasMoved(false), moveCounter(0) {
	switch (sym)
	{
	case 'P':
		type = TYPE::PAWN;
		color = Board::Board::COLOR::WHITE;
		break;
	case 'K':
		type = TYPE::KING;
		color = Board::Board::COLOR::WHITE;
		break;
	case 'B':
		type = TYPE::BISHOP;
		color = Board::COLOR::WHITE;
		break;
	case 'N':
		type = TYPE::KNIGHT;
		color = Board::COLOR::WHITE;
		break;
	case 'R':
		type = TYPE::ROOK;
		color = Board::COLOR::WHITE;
		break;
	case 'Q':
		type = TYPE::QUEEN;
		color = Board::COLOR::WHITE;
		break;
	case 'p':
		type = TYPE::PAWN;
		color = Board::COLOR::BLACK;
		break;
	case 'k':
		type = TYPE::KING;
		color = Board::COLOR::BLACK;
		break;
	case 'b':
		type = TYPE::BISHOP;
		color = Board::COLOR::BLACK;
		break;
	case 'n':
		type = TYPE::KNIGHT;
		color = Board::COLOR::BLACK;
		break;
	case 'r':
		type = TYPE::ROOK;
		color = Board::COLOR::BLACK; 
		break;
	case 'q':
		type = TYPE::QUEEN;
		color = Board::COLOR::BLACK;
		break;
	default:
		symbol = 'P';
		type = TYPE::PAWN;
		color = Board::COLOR::WHITE;
		break;
	}
}

Piece::Piece(char sym, Board::COLOR c, COORD p, int player) : symbol(sym), color(c), pos({ p.X, p.Y }), alive(true), ownerID(player), hasMoved(false), moveCounter(0) {
	switch (sym)
	{
	case 'P':
		type = TYPE::PAWN;
		break;
	case 'K':
		type = TYPE::KING;
		break;
	case 'B':
		type = TYPE::BISHOP;
		break;
	case 'N':
		type = TYPE::KNIGHT;
		break;
	case 'R':
		type = TYPE::ROOK;
		break;
	case 'Q':
		type = TYPE::QUEEN;
		break;
	}
}


Piece::Piece(const Piece& rhs) : color(rhs.color), type(rhs.type), symbol(rhs.symbol), alive(rhs.alive), hasMoved(rhs.hasMoved), moveCounter(rhs.moveCounter) {
	pos = { short(rhs.pos.X), short(rhs.pos.Y) };
}

Piece& Piece::operator=(const Piece& rhs) {
	color = rhs.color;
	type = rhs.type;
	symbol = rhs.symbol;
	alive = rhs.alive;
	pos = { short(rhs.pos.X), short(rhs.pos.Y) };
	moveCounter = rhs.moveCounter;
	ownerID = rhs.ownerID;
	return *this;
}

bool Piece::operator ==(const Piece& rhs) {
	return (type == rhs.type && color == rhs.color);
}

bool Piece::operator !=(const Piece& rhs) {
	return !(*this == rhs);
}

