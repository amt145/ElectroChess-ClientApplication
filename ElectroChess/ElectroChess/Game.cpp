/*
	File: Game.cpp
	Author: Alex Turner
	Last Revision Date: 04/24/2020
*/

// Fast Transfer piece IDs
// {p,r,n,b,q,k}=={1,2,3,4,5,6}

#pragma once
#include "Game.h"
#define BOARD_ADDR 6
#define CLIENT_ADDR 0
#define FT_MOVE_UNIT_LENGTH 6


Game::Game(int w, int h) : round(0), gameOver(false), ft(this, 0), hComm(NULL) {
	window = new Window(w, h);
	board = new Board(w, h);
	srand((int)time(0));
	Init();
}

Game::Game(int dim) : round(0), gameOver(false), ft(this, 0), hComm(NULL) {
	window = new Window(dim, dim);
	board = new Board(dim, dim);
	srand((int)time(0));
	Init();
}

Game::Game() : round(0), gameOver(false), ft(this, 0), hComm(NULL) {
	window = new Window(3, 3);
	board = new Board(3, 3);
	srand((int)time(0));
	Init();
}

HANDLE Game::GetWindowOutputHandle() const {
	return window->GetOutputHandle();
}

void Game::Update() {
	Turn();
	UpdateBoard();
	window->ClearTextBuffer();
}

void Game::Init() {
	board->Populate(Board::COLOR::RED, Board::COLOR::BLUE);
	UpdateBoard();
#ifndef C_DEBUG
	InitComms();
#endif
	InitPlayers();
}

void Game::UpdateBoard() {
	window->RefreshBoard();
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			window->SetChar(board->Grid()[i][j]->GetPiecePosition().X + window->GetLeftMarginBufferWidth(), board->Grid()[i][j]->GetPiecePosition().Y, board->Grid()[i][j]->GetPieceSymbol(), board->Grid()[i][j]->GetPieceColor());
		}
	}
	window->PrintBoard();
}

void Game::InitComms() {
	bytes = 0;
	bufEmpty = true;
	// If the comms handle has not been created, open default serial port for generic I/O
	if (hComm == NULL)
	{
		hComm = CreateFile("COM6",
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			0,
			NULL);
		if (hComm == INVALID_HANDLE_VALUE)
		{
			window->PrintLine("Error initializing communications");
			DWORD err = GetLastError();
			Pause();
		}
		// Setup device control structure to configure comm port for serial communications
		DCB dcbSerialParams = { 0 };
		dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
		GetCommState(hComm, &dcbSerialParams);
		dcbSerialParams.BaudRate = CBR_9600;
		dcbSerialParams.ByteSize = 8;
		dcbSerialParams.StopBits = ONESTOPBIT;
		dcbSerialParams.Parity = NOPARITY;
		SetCommState(hComm, &dcbSerialParams);

		// Set comms timeouts
		COMMTIMEOUTS timeouts = { 0 };
		timeouts.ReadIntervalTimeout = 50;
		timeouts.ReadTotalTimeoutConstant = 50;
		timeouts.ReadTotalTimeoutMultiplier = 10;
		timeouts.WriteTotalTimeoutConstant = 50;
		timeouts.WriteTotalTimeoutMultiplier = 10;
		SetCommTimeouts(hComm, &timeouts);
	}
}

void Game::InitComms(const char* portname) {
	bytes = 0;
	bufEmpty = true;
	// If the comms handle has not been created, open specified serial port for generic I/O
	if (hComm == NULL)
	{
		hComm = CreateFile(portname,
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			0,
			NULL);
		if (hComm == INVALID_HANDLE_VALUE)
		{
			window->PrintLine("Error initializing communications");
			DWORD err = GetLastError();
			Pause();
		}
		// Setup device control structure to configure comm port for serial communications
		DCB dcbSerialParams = { 0 };
		dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
		GetCommState(hComm, &dcbSerialParams);
		dcbSerialParams.BaudRate = CBR_9600;
		dcbSerialParams.ByteSize = 8;
		dcbSerialParams.StopBits = ONESTOPBIT;
		dcbSerialParams.Parity = NOPARITY;
		SetCommState(hComm, &dcbSerialParams);

		// Set comms timeouts
		COMMTIMEOUTS timeouts = { 0 };
		timeouts.ReadIntervalTimeout = 50;
		timeouts.ReadTotalTimeoutConstant = 50;
		timeouts.ReadTotalTimeoutMultiplier = 10;
		timeouts.WriteTotalTimeoutConstant = 50;
		timeouts.WriteTotalTimeoutMultiplier = 10;
		SetCommTimeouts(hComm, &timeouts);
	}
}


Game::FastTransfer::FastTransfer(Game* game, uint8_t address)
{
	// initialize local data:
	this->address = address;
	this->game = game;
	int i;
	for (i = 0; i < ARRAY_SZ; ++i) {
		this->arr[i] = 0;
		this->flags[i] = false;
	}

	// initialize receive/transmit buffers:
	Buffer_init(&this->receive_buf);
	Buffer_init(&this->transmit_buf);
}

int16_t Game::FastTransfer::FT_Read(uint8_t index) {
	this->flags[index] = false;
	return(int16_t)(this->arr[index]);
}

bool Game::FastTransfer::FT_Modified(uint8_t index) {
	bool temp = this->flags[index];
	this->flags[index] = false;
	return temp;
}

void Game::FastTransfer::FT_Receive() {
	unsigned char val, tmpVal;
	DWORD dwEventMask;
	SetCommMask(game->hComm, EV_RXCHAR);
	WaitCommEvent(game->hComm, &dwEventMask, NULL);
	game->bufEmpty = false;
	while (!game->BufferEmpty() && !Buffer_full(&this->receive_buf)) {
		val = game->BufferGet();
		this->receive_buf.data[(unsigned int)(this->receive_buf.tail)] = (unsigned char)val;
		this->receive_buf.tail = this->receive_buf.tail + 1;
		if (this->receive_buf.tail == MAX_PCKT_SZ)
			this->receive_buf.tail = 0;
		if (this->receive_buf.size <= MAX_PCKT_SZ)
			this->receive_buf.size = this->receive_buf.size + 1;
	}
	parse(&this->receive_buf, this->arr, this->flags, ARRAY_SZ, this->address);
}

void Game::FastTransfer::FT_ToSend(uint8_t index, int16_t data) {
	uint16_t combined = (uint16_t)data;
	uint8_t msb = (combined >> 8);
	uint8_t lsb = combined & 0x00FF;
	Buffer_push(&this->transmit_buf, index);
	Buffer_push(&this->transmit_buf, lsb);
	Buffer_push(&this->transmit_buf, msb);
}

void Game::FastTransfer::FT_Send(uint8_t address) {
	size_t payload_sz = Buffer_size(&this->transmit_buf);
	if (payload_sz == 0)
		return;
	game->BufferPut(0x06);
	game->BufferPut(0x85);
	game->BufferPut(address);
	game->BufferPut(this->address);
	game->BufferPut(payload_sz);

	uint8_t payload[MAX_PCKT_SZ];
	unsigned int i;
	for (i = 0; i < payload_sz; ++i) {
		uint8_t next_byte = Buffer_pop(&this->transmit_buf);
		payload[i] = next_byte;
		game->BufferPut(next_byte);
	}
	game->BufferPut(crc(payload, payload_sz));
}

void Game::BufferPut(uint8_t c) {
	char charBuf =  (char)c ;	
	DWORD numWritten = 0;
#ifndef RELEASE
	if (!WriteFile(hComm, &charBuf, 1, &numWritten, NULL))
	{
		window->PrintLine("failed to write file");
		DWORD err = GetLastError();
	}
#endif
#ifdef RELEASE
	WriteFile(hComm, &charBuf, 1, &numWritten, NULL);
#endif
}

uint8_t Game::BufferGet() {
	uint8_t tmp;
	ReadFile(hComm, &tmp, sizeof(tmp), &bytes, NULL);
	if (bytes == 0)
		bufEmpty = true;
	else
		bufEmpty = false;
	return (uint8_t)tmp;
}

std::string Game::RXBufferToCMD() {
	std::vector<uint16_t> v;
	std::string s;
	char type = 'e';
		for (int i = 0; i < ARRAY_SZ/2+1; ++i) {
			v.push_back(ft.FT_Read(i));
		}
		switch (v[3])
		{
		case 1:
			type = 'P';
			break;
		case 2:
			type = 'R';
			break;
		case 3:
			type = 'N';
			break;
		case 4:
			type = 'B';
			break;
		case 5:
			type = 'Q';
			break;
		case 6:
			type = 'K';
			break;
		}
		s.push_back(type);
		s.push_back(char(v[1] + 65));
		s.push_back(char(v[2] + 48));
		s.push_back(' ');
		s.push_back(char(v[4] + 65));
		s.push_back(char(v[5] + 48));
	return s;
}

void Game::InitPlayers() {
	Beep(450, 200);
	Beep(550, 200);
	Beep(650, 200);
	Beep(550, 200);
	
	// The player to go first is determined by a coin toss since the array of player objects is of length 2
	int coinToss = rand() % 2;
	window->PrintString("Welcome to Electro-Chess.");
	Sleep(4500);
	window->ClearTextBuffer();
	players.push_back(new Player("Player", 0, Board::COLOR::WHITE));
	players.push_back(new Player("Opponent", 1, Board::COLOR::BLACK));
	players[coinToss]->SetTurn(true);
	players[!coinToss]->SetTurn(false);

	/*
		Just like in classic chess, the first player to go has the white pieces.
		The player that lost the coin toss and goes second, get the black pieces.
		However, since the win32 console api only offers 16 different colors, 
		which did not offer colors close to the wood stain colors of a common chess
		board, a white and light grey were the colors chosen to represent the squares
		of the board. Therefore, the contrast between the squares and the pieces was
		so bad that the pieces could not be easily seen. Thus the colors of the pieces
		were chosen to be white->blue and black->red. The color choice was arbitrary
		other than the fact that red and blue offered the best contrast.
	*/
	if (players[0]->GetColor() == Board::COLOR::WHITE)
		board->Populate(Board::COLOR::RED, Board::COLOR::BLUE);
	else
		board->Populate(Board::COLOR::BLUE, Board::COLOR::RED);

	window->ClearTextBuffer();
}

// Main application game loop
int Game::Run() {
	// Game loop. While win32 console events are being processed and end game conditions are unmet, continue to update game
	while (window->ProcessMessages() && !gameOver) {
		Update();
	}
	auto i = players.begin();
	for (; i != players.end(); ++i)
		if ((*i)->IsWinner())
			break;
	window->ClearTextBuffer();
	if ((*i)->GetID() == 0)
		window->PrintLine("Game over, you win.");
	else
		window->PrintLine("Game over, your opponent wins.");
	Sleep(10000);
	return (int)true;
}



COORD Game::TranslateCommandCoords(std::string str) {
	int x = -1, y = -1;
	switch (str[0])
	{
	case 'A':
	case 'a':
		x = 0;
		break;
	case 'B':
	case 'b':
		x = 1;
		break;
	case 'C':
	case 'c':
		x = 2;
		break;
	case 'D':
	case 'd':
		x = 3;
		break;
	case 'E':
	case 'e':
		x = 4;
		break;
	case 'F':
	case 'f':
		x = 5;
		break;
	case 'G':
	case 'g':
		x = 6;
		break;
	case 'H':
	case 'h':
		x = 7;
		break;
	}
	y = str[1] - 48;
	y = abs(y - 8);
	return { short(x), short(y) };
}

int16_t* Game::BuildMoveBuffer(COORD src, COORD dest, int ts, int td) {
	moveBuffer[0] = int16_t(0);
	moveBuffer[1] = int16_t(src.X);
	moveBuffer[2] = int16_t(src.Y);
	moveBuffer[3] = int16_t(ts);
	moveBuffer[4] = int16_t(dest.X);
	moveBuffer[5] = int16_t(dest.Y);
	moveBuffer[6] = int16_t(td);
	return moveBuffer;
}

void Game::SendMove(int16_t* buf) {
	for (uint8_t i = 0; i < MOVE_SZ; ++i)
		ft.FT_ToSend(i, *(buf++));
	ft.FT_Send(BOARD_ADDR);
}

void Game::SendErrorMove() {
	for (uint8_t i = 0; i < MOVE_SZ; ++i)
		ft.FT_ToSend(i, -1);
	ft.FT_Send(BOARD_ADDR);
}

bool Game::EnPassant(std::vector<Player*>& players, Player& player, COORD p1, COORD p2) {
	if (board->Grid()[p2.Y][p2.X]->IsOccupied())
		return false;
	std::vector<Piece*>::iterator p = player.GetPieceList().begin();
	for (; p != player.GetPieceList().end(); ++p) {
		if ((*p)->GetSymbol() == 'P' && (*p)->GetPosition().X == p1.X && (*p)->GetPosition().Y == p1.Y)
			break;
	}
	(*p)->SetCurrentMovePattern(p1, *board);
	for (std::vector<COORD>::iterator i = (*p)->GetCurrentMovePattern().begin(); i != (*p)->GetCurrentMovePattern().end(); ++i) {
		if (i->X == p2.X && i->Y == p2.Y)
			return false;
	}
	if (player.GetID() == 0)
	{
		if (p2.Y == short(2) && p1.Y == short(3))
		{
			if (p1.X - short(1) >= Board::BoardDims::left && board->Grid()[p1.Y][p1.X - short(1)]->GetPieceSymbol() == 'P')
			{
				std::vector<Piece*>::iterator other = players[1]->GetPieceList().begin();
				for (; other != players[1]->GetPieceList().end(); ++other) {
					if ((*other)->GetPosition().X == p1.X - short(1) && (*other)->GetPosition().Y == p1.Y && (*other)->GetSymbol() == 'P')
						break;
				}
				if (other != players[1]->GetPieceList().end()
					&& (*other)->MoveCounter() == 1
					&& p2.X == p1.X - short(1)
					&& p2.Y == p1.Y - short(1))
				{
					board->Grid()[p2.Y + 1][p2.X]->RemovePiece();
					board->Grid()[p2.Y + 1][p2.X]->Vacant();
					board->Grid()[p1.Y][p1.X]->RemovePiece();
					board->Grid()[p1.Y][p1.X]->Vacant();
					board->Grid()[p2.Y][p2.X]->Place((*p)->GetSymbol(), (*p)->GetColor());
					player.AddCapturedPiece(new Pawn(player.GetColor(), player.GetID()));
					COORD deadPieceCoords = { (*other)->GetPosition().X, (*other)->GetPosition().Y };
					players[1]->RemovePiece(*other);
					(*p)->MoveTo({ p2.X, p2.Y });
					SendMove(BuildMoveBuffer(p1, p2, 1, 1));
					SendMove(BuildMoveBuffer(deadPieceCoords, { short(8), short(8) }, 1, 1));
					return true;
				}
			}
			else if (p1.X + short(1) <= Board::BoardDims::right && board->Grid()[p1.Y][p1.X + short(1)]->GetPieceSymbol() == 'P')
			{
				std::vector<Piece*>::iterator other = players[1]->GetPieceList().begin();
				for (; other != players[1]->GetPieceList().end(); ++other) {
					if ((*other)->GetPosition().X == p1.X + short(1) && (*other)->GetPosition().Y == p1.Y && (*other)->GetSymbol() == 'P')
						break;
				}
				if (other != players[1]->GetPieceList().end()
					&& (*other)->MoveCounter() == 1
					&& p2.X == p1.X + short(1)
					&& p2.Y == p1.Y - short(1))
				{
					board->Grid()[p2.Y + 1][p2.X]->RemovePiece();
					board->Grid()[p2.Y + 1][p2.X]->Vacant();
					board->Grid()[p1.Y][p1.X]->RemovePiece();
					board->Grid()[p1.Y][p1.X]->Vacant();
					board->Grid()[p2.Y][p2.X]->Place((*p)->GetSymbol(), (*p)->GetColor());
					player.AddCapturedPiece(new Pawn(player.GetColor(), player.GetID()));
					COORD deadPieceCoords = { (*other)->GetPosition().X, (*other)->GetPosition().Y };
					players[1]->RemovePiece(*other);
					(*p)->MoveTo({ p2.X, p2.Y });
					SendMove(BuildMoveBuffer(p1, p2, 1, 1));
					SendMove(BuildMoveBuffer(deadPieceCoords, { short(8), short(8) }, 1, 1));
					return true;
				}
			}
		}
	}
	else if (player.GetID() == 1)
	{
		if (p2.Y == short(3) && p1.Y == short(4))
		{
			if (p1.X - short(1) >= Board::BoardDims::left && board->Grid()[p1.Y][p1.X - short(1)]->GetPieceSymbol() == 'P')
			{
				std::vector<Piece*>::iterator other = players[0]->GetPieceList().begin();
				for (; other != players[0]->GetPieceList().end(); ++other) {
					if ((*other)->GetPosition().X == p1.X - short(1) && (*other)->GetPosition().Y == p1.Y && (*other)->GetSymbol() == 'P')
						break;
				}
				if (other != players[0]->GetPieceList().end()
					&& (*other)->MoveCounter() == 1
					&& p2.X == p1.X - short(1)
					&& p2.Y == p1.Y - short(1))
				{
					board->Grid()[p2.Y - 1][p2.X]->RemovePiece();
					board->Grid()[p2.Y - 1][p2.X]->Vacant();
					board->Grid()[p1.Y][p1.X]->RemovePiece();
					board->Grid()[p1.Y][p1.X]->Vacant();
					board->Grid()[p2.Y][p2.X]->Place((*p)->GetSymbol(), (*p)->GetColor());
					player.AddCapturedPiece(new Pawn(player.GetColor(), player.GetID()));
					COORD deadPieceCoords = { (*other)->GetPosition().X, (*other)->GetPosition().Y };
					players[0]->RemovePiece(*other);
					(*p)->MoveTo({ p2.X, p2.Y });
					return true;
				}
			}
			else if (p1.X + short(1) <= Board::BoardDims::right && board->Grid()[p1.Y][p1.X + short(1)]->GetPieceSymbol() == 'P')
			{
				std::vector<Piece*>::iterator other = players[0]->GetPieceList().begin();
				for (; other != players[0]->GetPieceList().end(); ++other) {
					if ((*other)->GetPosition().X == p1.X + short(1) && (*other)->GetPosition().Y == p1.Y && (*other)->GetSymbol() == 'P')
						break;
				}
				if (other != players[0]->GetPieceList().end()
					&& (*other)->MoveCounter() == 1
					&& p2.X == p1.X + short(1)
					&& p2.Y == p1.Y - short(1))
				{
					board->Grid()[p2.Y - 1][p2.X]->RemovePiece();
					board->Grid()[p2.Y - 1][p2.X]->Vacant();
					board->Grid()[p1.Y][p1.X]->RemovePiece();
					board->Grid()[p1.Y][p1.X]->Vacant();
					board->Grid()[p2.Y][p2.X]->Place((*p)->GetSymbol(), (*p)->GetColor());
					player.AddCapturedPiece(new Pawn(player.GetColor(), player.GetID()));
					COORD deadPieceCoords = { (*other)->GetPosition().X, (*other)->GetPosition().Y };
					players[1]->RemovePiece(*other);
					(*p)->MoveTo({ p2.X, p2.Y });
					return true;
				}
			}
		}
	}
	else
		return false;
}

void Game::CheckKing(std::vector<Player*>& players, Player& player, COORD p2) {
	std::vector<Piece*>::iterator i = player.GetPieceList().begin();
	int otherID = player.GetID() == 0 ? 1 : 0;
	std::vector<Piece*>::iterator j = players[otherID]->GetPieceList().begin();
	for (; j != players[otherID]->GetPieceList().end(); ++j)
		if ((*j)->GetSymbol() == 'K')
			break;
	COORD otherKingCoord = { (*j)->GetPosition().X, (*j)->GetPosition().Y };
	for (; i != player.GetPieceList().end(); ++i)
		if ((*i)->GetPosition().X == p2.X && (*i)->GetPosition().Y == p2.Y)
			break;
	(*i)->SetCurrentMovePattern({ p2.X, p2.Y }, *board);
	(*i)->SetCurrentCapturePattern({ p2.X, p2.Y }, *board);
	std::vector<COORD>::iterator k = (*i)->GetCurrentCapturePattern().begin();
	for (; k != (*i)->GetCurrentCapturePattern().end(); ++k) {
		if (k->X == otherKingCoord.X && k->Y == otherKingCoord.Y)
		{
			(*j)->Check();
			break;
		}
	}
	k = (*i)->GetCurrentCapturePattern().begin();
}

bool Game::MovePiece(Player& player, std::string command) {
	bool validMove = false;


	// Check if move was a castle
	int castleType = -1;
	if (command[0] == '0' || command[0] == 'O' || command[0] == 'o')
	{
		int k = 0;
		for (int i = 0; i < command.length(); i++) {
			if (command[i] == '0' || command[i] == 'O' || command[i] == 'o')
				k++;
		}
		if (k == 2)
			castleType = 0; // 0-0 (kingside)
		else if (k == 3)
			castleType = 1; // 0-0-0 (queenside)
		if (player.GetID() == 0)
		{
			bool canCastle = false;
			auto list1 = player.GetPieceList().begin();
			for (; list1 != player.GetPieceList().end(); ++list1) {
				if ((*list1)->GetType() == Piece::TYPE::KING && (*list1)->GetPosition().X == short(4) && (*list1)->GetPosition().Y == short(7))
				{
					canCastle = true;
					break;
				}
			}
			if (canCastle)
			{
				if (board->Grid()[7][4]->GetPieceSymbol() == 'K'
					&& board->Grid()[7][4]->GetPlayerColor() == player.GetColor()
					&& !(*list1)->HasMoved())
				{
					if (castleType == 0)
					{
						auto list2 = player.GetPieceList().begin();
						for (; list2 != player.GetPieceList().end(); ++list2) {
							if ((*list2)->GetType() == Piece::TYPE::ROOK && (*list2)->GetPosition().X == short(7) && (*list2)->GetPosition().Y == short(7))
								break;
						}
						if (   board->Grid()[7][7]->GetPieceSymbol() == 'R'
							&& board->Grid()[7][7]->GetPlayerColor() == player.GetColor()
							&& !board->Grid()[7][5]->IsOccupied()
							&& !board->Grid()[7][6]->IsOccupied()
							&& !(*list2)->HasMoved())
						{
							(*list1)->MoveTo({ short(6), short(7) });
							(*list2)->MoveTo({ short(5), short(7) });
							board->Grid()[7][7]->RemovePiece();
							board->Grid()[7][4]->RemovePiece();
							board->Grid()[7][6]->Place((*list1)->GetSymbol(), (*list1)->GetColor());
							board->Grid()[7][5]->Place((*list2)->GetSymbol(), (*list2)->GetColor());
							//send king on (4,7) to (6,7)
							ft.FT_ToSend(1, 4);
							ft.FT_ToSend(2, 7);
							ft.FT_ToSend(3, 6);

							ft.FT_ToSend(4, 6);
							ft.FT_ToSend(5, 7);
							ft.FT_ToSend(6, 6);
							ft.FT_Send(BOARD_ADDR);

							//wait for confirmation
							ft.FT_Receive();

							//send rook on (7,7) to (5,7)
							ft.FT_ToSend(1, 7);
							ft.FT_ToSend(2, 7);
							ft.FT_ToSend(3, 2);

							ft.FT_ToSend(4, 5);
							ft.FT_ToSend(5, 7);
							ft.FT_ToSend(6, 2);
							ft.FT_Send(BOARD_ADDR);

							return true;
						}
					}
					else if (castleType == 1)
					{
						auto list3 = player.GetPieceList().begin();
						for (; list3 != player.GetPieceList().end(); ++list3) {
							if ((*list3)->GetType() == Piece::TYPE::ROOK && (*list3)->GetPosition().X == short(0) && (*list3)->GetPosition().Y == short(7))
								break;
						}
						if (   board->Grid()[7][0]->GetPieceSymbol() == 'R'
							&& board->Grid()[7][0]->GetPlayerColor() == player.GetColor()
							&& !board->Grid()[7][2]->IsOccupied()
							&& !board->Grid()[7][3]->IsOccupied()
							&& !(*list3)->HasMoved())
						{
							(*list1)->MoveTo({ short(2), short(7) });
							(*list3)->MoveTo({ short(3), short(7) });
							board->Grid()[7][0]->RemovePiece();
							board->Grid()[7][4]->RemovePiece();
							board->Grid()[7][2]->Place((*list1)->GetSymbol(), (*list1)->GetColor());
							board->Grid()[7][3]->Place((*list3)->GetSymbol(), (*list3)->GetColor());
							//send king on (4,7) to (2,7)
							ft.FT_ToSend(0, 4);
							ft.FT_ToSend(1, 7);
							ft.FT_ToSend(2, 5);

							ft.FT_ToSend(3, 2);
							ft.FT_ToSend(4, 7);
							ft.FT_ToSend(5, 5);
							ft.FT_Send(BOARD_ADDR);

							//wait for confirmation
							ft.FT_Receive();
							

							//send rook on (0,7) to (3,7)
							ft.FT_ToSend(0, 0);
							ft.FT_ToSend(1, 7);
							ft.FT_ToSend(2, 1);

							ft.FT_ToSend(3, 3);
							ft.FT_ToSend(4, 7);
							ft.FT_ToSend(5, 1);
							ft.FT_Send(BOARD_ADDR);

							return true;
						}
					}
					else
						return false;
				}
				else
					return false;
			}
			else
				return false;
		}
		else if (player.GetID() == 1)
		{
			bool canCastle = false;
			auto list1 = player.GetPieceList().begin();
			for (; list1 != player.GetPieceList().end(); ++list1) {
				if ((*list1)->GetType() == Piece::TYPE::KING && (*list1)->GetPosition().X == short(4) && (*list1)->GetPosition().Y == short(0))
				{
					canCastle = true;
					break;
				}
			}
			if (canCastle)
			{
				if (board->Grid()[0][4]->GetPieceSymbol() == 'K'
					&& board->Grid()[0][4]->GetPlayerColor() == player.GetColor()
					&& !(*list1)->HasMoved())
				{
					if (castleType == 0)
					{
						auto list2 = player.GetPieceList().begin();
						for (; list2 != player.GetPieceList().end(); ++list2) {
							if ((*list2)->GetType() == Piece::TYPE::ROOK && (*list2)->GetPosition().X == short(7) && (*list2)->GetPosition().Y == short(0))
								break;
						}
						if (board->Grid()[0][7]->GetPieceSymbol() == 'R'
							&& board->Grid()[0][7]->GetPlayerColor() == player.GetColor()
							&& !board->Grid()[0][5]->IsOccupied()
							&& !board->Grid()[0][6]->IsOccupied()
							&& !(*list2)->HasMoved())
						{
							(*list1)->MoveTo({ short(6), short(0) });
							(*list2)->MoveTo({ short(5), short(0) });
							board->Grid()[0][7]->RemovePiece();
							board->Grid()[0][4]->RemovePiece();
							board->Grid()[0][6]->Place((*list1)->GetSymbol(), (*list1)->GetColor());
							board->Grid()[0][5]->Place((*list2)->GetSymbol(), (*list2)->GetColor());
							return true;
						}
					}
					else if (castleType == 1)
					{
						auto list3 = player.GetPieceList().begin();
						for (; list3 != player.GetPieceList().end(); ++list3) {
							if ((*list3)->GetType() == Piece::TYPE::ROOK && (*list3)->GetPosition().X == short(0) && (*list3)->GetPosition().Y == short(0))
								break;
						}
						if (board->Grid()[0][0]->GetPieceSymbol() == 'R'
							&& board->Grid()[0][0]->GetPlayerColor() == player.GetColor()
							&& !board->Grid()[0][2]->IsOccupied()
							&& !board->Grid()[0][3]->IsOccupied()
							&& !(*list3)->HasMoved())
						{
							(*list1)->MoveTo({ short(2), short(0) });
							(*list3)->MoveTo({ short(3), short(0) });
							board->Grid()[0][0]->RemovePiece();
							board->Grid()[0][4]->RemovePiece();
							board->Grid()[0][2]->Place((*list1)->GetSymbol(), (*list1)->GetColor());
							board->Grid()[0][3]->Place((*list3)->GetSymbol(), (*list3)->GetColor());
							return true;
						}
					}
					else
						return false;
				}
				else
					return false;
			}
		}
		else
			return false;
		return false;
	}
	std::istringstream iss(command.substr(1,command.length()-1));
	std::string from, to;
	
	iss >> from;
	iss >> to;
	
	if (from.size() != 2 || to.size() != 2)
		return false;
	
	COORD p1 = TranslateCommandCoords(from);
	COORD p2 = TranslateCommandCoords(to);

	if (p1.X < Board::BoardDims::left || p1.Y < Board::BoardDims::top || p1.X > Board::BoardDims::right || p1.Y > Board::BoardDims::bottom)
		return false;
	if (p2.X < Board::BoardDims::left || p2.Y < Board::BoardDims::top || p2.X > Board::BoardDims::right || p2.Y > Board::BoardDims::bottom)
		return false;

	std::vector<Piece*>::iterator playerKing = player.GetPieceList().begin();
	for (; playerKing != player.GetPieceList().end(); ++playerKing)
		if ((*playerKing)->GetSymbol() == 'K')
			break;

	if (playerKing != player.GetPieceList().end() && (*playerKing)->IsCheck())
	{
		if (player.GetID() == 0)
		{
			window->ClearTextBuffer();
			window->PrintLine("You must move your king out of check!");
			Sleep(1000);
			window->ClearTextBuffer();
		}
		else
		{
#if defined(C_DEBUG) || defined(CB_DEBUG)
			window->ClearTextBuffer();
			window->PrintLine("You must move your king out of check!");
			Sleep(1000);
			window->ClearTextBuffer();
#endif
		}

		return false;
	}

	if (!isupper(command[0]))
	{
		char c = command[0];
		command[0] = toupper(c);
	}

	bool enPassant = false;
	bool stdCapture = false;
	bool promoting = false;
	int captureID = -1;
	int otherID = player.GetID() == 0 ? 1 : 0;
	std::vector<Piece*>::iterator i = player.GetPieceList().begin();
	std::vector<Piece*>::iterator other = players[otherID]->GetPieceList().begin();
	COORD otherKingCoord;

	for (; other != players[otherID]->GetPieceList().end(); ++other) {
		if ((*other)->GetSymbol() == 'K')
			break;
	}
	otherKingCoord = { short((*other)->GetPosition().X), short((*other)->GetPosition().Y) };

	for (; i != player.GetPieceList().end(); ++i) {
		if ((*i)->GetSymbol() == command[0] && (*i)->GetPosition().X == p1.X && (*i)->GetPosition().Y == p1.Y)
			break;
	}
	if (i == player.GetPieceList().end())
		return false;

	(*i)->SetCurrentMovePattern({ p1.X, p1.Y }, *board);
	std::vector<COORD>::iterator k;
	(*i)->SetCurrentCapturePattern({ p1.X, p1.Y }, * board);
	i = player.GetPieceList().begin();
	std::vector<COORD>::iterator j = (*i)->GetCurrentMovePattern().begin();

	switch (command[0])
	{
	case 'P':
	case 'p':
		captureID = player.GetID() == 0 ? 1 : 0;
		enPassant = false;
		i = player.GetPieceList().begin();
		for ( ; i != player.GetPieceList().end(); ++i) {
			if ((*i)->GetSymbol() == 'P' && (*i)->GetPosition().X == p1.X && (*i)->GetPosition().Y == p1.Y)
				break;
		}
		if (i == player.GetPieceList().end())
			return false;
		
		(*i)->SetCurrentMovePattern(p1, *board);
		j = (*i)->GetCurrentMovePattern().begin();
		for (; j != (*i)->GetCurrentMovePattern().end(); ++j) {
			if (j->X == p2.X && j->Y == p2.Y)
			{
				stdCapture = false;
				break;
			}
			else
				stdCapture = true;
		}
		if (stdCapture == false)
		{
			promoting = false;
			if (player.GetID() == 0)
			{
				if (p2.Y == 0)
				{
					(*i)->MoveTo(p2);
					Promote(*players[0], { p2.X, p2.Y });
					int tp = board->Grid()[p2.Y][p2.X]->GetPieceEnumeration();
#ifndef C_DEBUG
					SendMove(BuildMoveBuffer(p1, p2, 1, tp));
#endif
					CheckKing(players, player, p2);
					promoting = true;
				}
				else
				{
					promoting = false;
				}
			}
			else // pid=1
			{
				if (p2.Y == 7)
				{
					(*i)->MoveTo(p2);
					Promote(*players[1], { p2.X, p2.Y });
#ifdef CB_DEBUG
					int tp = board->Grid()[p2.Y][p2.X]->GetPieceEnumeration();
					SendMove(BuildMoveBuffer(p1, p2, 1, tp));
#endif
					CheckKing(players, player, p2);
					promoting = true;
				}
				else
				{
					promoting = false;
				}
			}
			if (promoting == false)
			{
				board->Grid()[p1.Y][p1.X]->RemovePiece();
				board->Grid()[p1.Y][p1.X]->Vacant();
				board->Grid()[p2.Y][p2.X]->Place((*i)->GetSymbol(), (*i)->GetColor());
				(*i)->MoveTo(p2);
				CheckKing(players, player, p2);
#if !defined(C_DEBUG) && defined(CB_DEBUG)
				if (player.GetID() == 0)
#ifndef C_DEBUG
					SendMove(BuildMoveBuffer(p1, p2, 1, 1));
#endif
				else
#ifdef CB_DEBUG
					SendMove(BuildMoveBuffer(p1, p2, 1, 1));
#endif
#endif
			}
			validMove = true;
			stdCapture = false;
		}
		if (stdCapture)
		{
			k = (*i)->GetCurrentCapturePattern().begin();
			captureID = -1;
			bool found = false;
			for (; k != (*i)->GetCurrentCapturePattern().end(); ++k) {
				if (k->X == p2.X && k->Y == p2.Y && board->Grid()[p2.Y][p2.X]->IsOccupied() && board->Grid()[p2.Y][p2.X]->GetPlayerColor() != player.GetColor())
				{
					found = true;
					break;
				}
			}
			if (found)
				enPassant = EnPassant(players, player, p1, p2);

			// standard capture
			if (enPassant == false && p1.X != p2.X)
			{
				(*i)->MoveTo({ p2.X, p2.Y });
				switch (board->Grid()[p2.Y][p2.X]->GetPieceSymbol())
				{
				case 'P':
					player.AddCapturedPiece(new Pawn(player.GetColor(), player.GetID()));
					break;
				case 'R':
					player.AddCapturedPiece(new Rook(player.GetColor(), player.GetID()));
					break;
				case 'N':
					player.AddCapturedPiece(new Knight(player.GetColor(), player.GetID()));
					break;
				case 'B':
					player.AddCapturedPiece(new Bishop(player.GetColor(), player.GetID()));
					break;
				case 'Q':
					player.AddCapturedPiece(new Queen(player.GetColor(), player.GetID()));
					break;
				case 'K':
					gameOver = true;
					break;
				}
				promoting = false;
				if (player.GetID() == 0)
				{
					if (p2.Y == 0)
					{
						Promote(*players[0], { p2.X, p2.Y });
						int tp = board->Grid()[p2.Y][p2.X]->GetPieceEnumeration();
#ifndef C_DEBUG
						SendMove(BuildMoveBuffer(p1, p2, 1, tp));
#endif
						CheckKing(players, player, p2);
						promoting = true;
						validMove = true;
					}
					else
						promoting = false;
				}
				else
				{
					if (p2.Y == 7)
					{
						Promote(*players[1], { p2.X, p2.Y });
#ifdef CB_DEBUG
						int tp = board->Grid()[p2.Y][p2.X]->GetPieceEnumeration();
						SendMove(BuildMoveBuffer(p1, p2, 1, tp));
#endif
						CheckKing(players, player, p2);
						promoting = true;
						validMove = true;
					}
					else
						promoting = false;
				}
				if (promoting == false)
				{
					players[otherID]->RemovePiece({ p2.X, p2.Y });
					board->Grid()[p1.Y][p1.X]->RemovePiece();
					board->Grid()[p1.Y][p1.X]->Vacant();
					board->Grid()[p2.Y][p2.X]->Place((*i)->GetSymbol(), player.GetColor());
#ifdef CB_DEBUG
					int tp = board->Grid()[p2.Y][p2.X]->GetPieceEnumeration();
					SendMove(BuildMoveBuffer(p1, p2, 1, tp));
#endif
					CheckKing(players, player, p2);
					validMove = true;
				}
			}
		}
		break;
	case 'R':
	case 'r':
		captureID = -1;
		captureID = player.GetID() == 0 ? 1 : 0;
		i = player.GetPieceList().begin();
		for (; i != player.GetPieceList().end(); ++i) {
			if ((*i)->GetSymbol() == 'R' && (*i)->GetPosition().X == p1.X && (*i)->GetPosition().Y == p1.Y)
				break;
		}
		if (i == player.GetPieceList().end())
			return false;
		j = (*i)->GetCurrentMovePattern().begin();
		for (; j != (*i)->GetCurrentMovePattern().end(); ++j) {
			if (j->X == p2.X && j->Y == p2.Y)
			{
				if (board->Grid()[p2.Y][p2.X]->IsOccupied())
				{
#if !defined(C_DEBUG) && defined(CB_DEBUG)
						if (player.GetID() == 0)
#ifndef C_DEBUG
							SendMove(BuildMoveBuffer(p1, p2, 2, 2));
#endif
						else
#ifdef CB_DEBUG
							SendMove(BuildMoveBuffer(p1, p2, 2, 2));
#endif
#endif
					switch (board->Grid()[p2.Y][p2.X]->GetPieceSymbol())
					{
					case 'P':
						player.AddCapturedPiece(new Pawn(player.GetColor(), player.GetID()));
						break;
					case 'R':
						player.AddCapturedPiece(new Rook(player.GetColor(), player.GetID()));
						break;
					case 'N':
						player.AddCapturedPiece(new Knight(player.GetColor(), player.GetID()));
						break;
					case 'B':
						player.AddCapturedPiece(new Bishop(player.GetColor(), player.GetID()));
						break;
					case 'Q':
						player.AddCapturedPiece(new Queen(player.GetColor(), player.GetID()));
						break;
					case 'K':
						gameOver = true;
						break;
					}
					players[captureID]->RemovePiece({ p2.X, p2.Y });
					(*i)->MoveTo({ p2.X, p2.Y });
					board->Grid()[p1.Y][p1.X]->RemovePiece();
					board->Grid()[p1.Y][p1.X]->Vacant();
					board->Grid()[p2.Y][p2.X]->Place((*i)->GetSymbol(), player.GetColor());
					CheckKing(players, player, p2);
					validMove = true;
					break;
				}
				else
				{
					(*i)->MoveTo({ p2.X, p2.Y });
#if !defined(C_DEBUG) && defined(CB_DEBUG)
					if (player.GetID() == 0)
#ifndef C_DEBUG
						SendMove(BuildMoveBuffer(p1, p2, 2, 2));
#endif
					else
#ifdef CB_DEBUG
						SendMove(BuildMoveBuffer(p1, p2, 2, 2));
#endif
#endif
					board->Grid()[p1.Y][p1.X]->RemovePiece();
					board->Grid()[p1.Y][p1.X]->Vacant();
					board->Grid()[p2.Y][p2.X]->Place((*i)->GetSymbol(), (*i)->GetColor());
					CheckKing(players, player, p2);
					validMove = true;
					break;
				}
			}
		}
		break;
	case 'N':
	case 'n':
		captureID = -1;
		captureID = player.GetID() == 0 ? 1 : 0;
		i = player.GetPieceList().begin();
		for (; i != player.GetPieceList().end(); ++i) {
			if ((*i)->GetSymbol() == 'N' && (*i)->GetPosition().X == p1.X && (*i)->GetPosition().Y == p1.Y)
				break;
		}
		if (i == player.GetPieceList().end())
			return false;
		j = (*i)->GetCurrentMovePattern().begin();
		for (; j != (*i)->GetCurrentMovePattern().end(); ++j) {
			if (j->X == p2.X && j->Y == p2.Y)
			{
				if (board->Grid()[p2.Y][p2.X]->IsOccupied())
				{
#if !defined(C_DEBUG) && defined(CB_DEBUG)
					if (player.GetID() == 0)
#ifndef C_DEBUG
						SendMove(BuildMoveBuffer(p1, p2, 3, 3));
#endif
					else
#ifdef CB_DEBUG
						SendMove(BuildMoveBuffer(p1, p2, 3, 3));
#endif
#endif
					switch (board->Grid()[p2.Y][p2.X]->GetPieceSymbol())
					{
					case 'P':
						player.AddCapturedPiece(new Pawn(player.GetColor(), player.GetID()));
						break;
					case 'R':
						player.AddCapturedPiece(new Rook(player.GetColor(), player.GetID()));
						break;
					case 'N':
						player.AddCapturedPiece(new Knight(player.GetColor(), player.GetID()));
						break;
					case 'B':
						player.AddCapturedPiece(new Bishop(player.GetColor(), player.GetID()));
						break;
					case 'Q':
						player.AddCapturedPiece(new Queen(player.GetColor(), player.GetID()));
						break;
					case 'K':
						gameOver = true;
						break;
					}
					players[captureID]->RemovePiece({ p2.X, p2.Y });
					(*i)->MoveTo({ p2.X, p2.Y });
					board->Grid()[p1.Y][p1.X]->RemovePiece();
					board->Grid()[p1.Y][p1.X]->Vacant();
					board->Grid()[p2.Y][p2.X]->Place((*i)->GetSymbol(), player.GetColor());
					CheckKing(players, player, p2);
					validMove = true;
					break;
				}
				else
				{
					(*i)->MoveTo({ p2.X, p2.Y });
#if !defined(C_DEBUG) && defined(CB_DEBUG)
					if (player.GetID() == 0)
#ifndef C_DEBUG
						SendMove(BuildMoveBuffer(p1, p2, 3, 3));
#endif
					else
#ifdef CB_DEBUG
						SendMove(BuildMoveBuffer(p1, p2, 3, 3));
#endif
#endif
					board->Grid()[p1.Y][p1.X]->RemovePiece();
					board->Grid()[p1.Y][p1.X]->Vacant();
					board->Grid()[p2.Y][p2.X]->Place((*i)->GetSymbol(), (*i)->GetColor());
					CheckKing(players, player, p2);
					validMove = true;
					break;
				}
			}
		}
		break;
	case 'B':
	case 'b':
		captureID = -1;
		captureID = player.GetID() == 0 ? 1 : 0;
		i = player.GetPieceList().begin();
		for (; i != player.GetPieceList().end(); ++i) {
			if ((*i)->GetSymbol() == 'B' && (*i)->GetPosition().X == p1.X && (*i)->GetPosition().Y == p1.Y)
				break;
		}
		if (i == player.GetPieceList().end())
			return false;
		j = (*i)->GetCurrentMovePattern().begin();
		for (; j != (*i)->GetCurrentMovePattern().end(); ++j) {
			if (j->X == p2.X && j->Y == p2.Y)
			{
				if (board->Grid()[p2.Y][p2.X]->IsOccupied())
				{
#if !defined(C_DEBUG) && defined(CB_DEBUG)
					if (player.GetID() == 0)
#ifndef C_DEBUG
						SendMove(BuildMoveBuffer(p1, p2, 4, 4));
#endif
					else
#ifdef CB_DEBUG
						SendMove(BuildMoveBuffer(p1, p2, 4, 4));
#endif
#endif
					switch (board->Grid()[p2.Y][p2.X]->GetPieceSymbol())
					{
					case 'P':
						player.AddCapturedPiece(new Pawn(player.GetColor(), player.GetID()));
						break;
					case 'R':
						player.AddCapturedPiece(new Rook(player.GetColor(), player.GetID()));
						break;
					case 'N':
						player.AddCapturedPiece(new Knight(player.GetColor(), player.GetID()));
						break;
					case 'B':
						player.AddCapturedPiece(new Bishop(player.GetColor(), player.GetID()));
						break;
					case 'Q':
						player.AddCapturedPiece(new Queen(player.GetColor(), player.GetID()));
						break;
					case 'K':
						gameOver = true;
						break;
					}
					players[captureID]->RemovePiece({ p2.X, p2.Y });
					(*i)->MoveTo({ p2.X, p2.Y });
					board->Grid()[p1.Y][p1.X]->RemovePiece();
					board->Grid()[p1.Y][p1.X]->Vacant();
					board->Grid()[p2.Y][p2.X]->Place((*i)->GetSymbol(), player.GetColor());
					CheckKing(players, player, p2);
					validMove = true;
					break;
				}
				else
				{
					(*i)->MoveTo({ p2.X, p2.Y });
#if !defined(C_DEBUG) && defined(CB_DEBUG)
					if (player.GetID() == 0)
#ifndef C_DEBUG
						SendMove(BuildMoveBuffer(p1, p2, 4, 4));
#endif
					else
#ifdef CB_DEBUG
						SendMove(BuildMoveBuffer(p1, p2, 4, 4));
#endif
#endif
					board->Grid()[p1.Y][p1.X]->RemovePiece();
					board->Grid()[p1.Y][p1.X]->Vacant();
					board->Grid()[p2.Y][p2.X]->Place((*i)->GetSymbol(), (*i)->GetColor());
					CheckKing(players, player, p2);
					validMove = true;
					break;
				}
			}
		}
		break;
	case 'Q':
	case 'q':
		captureID = -1;
		captureID = player.GetID() == 0 ? 1 : 0;
		i = player.GetPieceList().begin();
		for (; i != player.GetPieceList().end(); ++i) {
			if ((*i)->GetSymbol() == 'Q' && (*i)->GetPosition().X == p1.X && (*i)->GetPosition().Y == p1.Y)
				break;
		}
		if (i == player.GetPieceList().end())
			return false;
		j = (*i)->GetCurrentMovePattern().begin();
		for (; j != (*i)->GetCurrentMovePattern().end(); ++j) {
			if (j->X == p2.X && j->Y == p2.Y)
			{
				if (board->Grid()[p2.Y][p2.X]->IsOccupied())
				{
#if !defined(C_DEBUG) && defined(CB_DEBUG)
					if (player.GetID() == 0)
#ifndef C_DEBUG
						SendMove(BuildMoveBuffer(p1, p2, 5, 5));
#endif
					else
#ifdef CB_DEBUG
						SendMove(BuildMoveBuffer(p1, p2, 5, 5));
#endif
#endif
					switch (board->Grid()[p2.Y][p2.X]->GetPieceSymbol())
					{
					case 'P':
						player.AddCapturedPiece(new Pawn(player.GetColor(), player.GetID()));
						break;
					case 'R':
						player.AddCapturedPiece(new Rook(player.GetColor(), player.GetID()));
						break;
					case 'N':
						player.AddCapturedPiece(new Knight(player.GetColor(), player.GetID()));
						break;
					case 'B':
						player.AddCapturedPiece(new Bishop(player.GetColor(), player.GetID()));
						break;
					case 'Q':
						player.AddCapturedPiece(new Queen(player.GetColor(), player.GetID()));
						break;
					case 'K':
						gameOver = true;
						break;
					}
					players[captureID]->RemovePiece({ p2.X, p2.Y });
					(*i)->MoveTo({ p2.X, p2.Y });
					board->Grid()[p1.Y][p1.X]->RemovePiece();
					board->Grid()[p1.Y][p1.X]->Vacant();
					board->Grid()[p2.Y][p2.X]->Place((*i)->GetSymbol(), player.GetColor());
					CheckKing(players, player, p2);
					validMove = true;
					break;
				}
				else
				{
					(*i)->MoveTo({ p2.X, p2.Y });
#if !defined(C_DEBUG) && defined(CB_DEBUG)
					if (player.GetID() == 0)
#ifndef C_DEBUG
						SendMove(BuildMoveBuffer(p1, p2, 5, 5));
#endif
					else
#ifdef CB_DEBUG
						SendMove(BuildMoveBuffer(p1, p2, 5, 5));
#endif
#endif
					board->Grid()[p1.Y][p1.X]->RemovePiece();
					board->Grid()[p1.Y][p1.X]->Vacant();
					board->Grid()[p2.Y][p2.X]->Place((*i)->GetSymbol(), (*i)->GetColor());
					CheckKing(players, player, p2);
					validMove = true;
					break;
				}
			}
		}
		break;
	case 'K':
	case 'k':
		captureID = -1;
		captureID = player.GetID() == 0 ? 1 : 0;
		i = player.GetPieceList().begin();
		for (; i != player.GetPieceList().end(); ++i) {
			if ((*i)->GetSymbol() == 'K' && (*i)->GetPosition().X == p1.X && (*i)->GetPosition().Y == p1.Y)
				break;
		}
		if (i == player.GetPieceList().end())
			return false;
		j = (*i)->GetCurrentMovePattern().begin();
		for (; j != (*i)->GetCurrentMovePattern().end(); ++j) {
			if (j->X == p2.X && j->Y == p2.Y)
			{
				if (board->Grid()[p2.Y][p2.X]->IsOccupied())
				{
#if !defined(C_DEBUG) && defined(CB_DEBUG)
					if (player.GetID() == 0)
#ifndef C_DEBUG
						SendMove(BuildMoveBuffer(p1, p2, 6, 6));
#endif
					else
#ifdef CB_DEBUG
						SendMove(BuildMoveBuffer(p1, p2, 6, 6));
#endif
#endif
					switch (board->Grid()[p2.Y][p2.X]->GetPieceSymbol())
					{
					case 'P':
						player.AddCapturedPiece(new Pawn(player.GetColor(), player.GetID()));
						break;
					case 'R':
						player.AddCapturedPiece(new Rook(player.GetColor(), player.GetID()));
						break;
					case 'N':
						player.AddCapturedPiece(new Knight(player.GetColor(), player.GetID()));
						break;
					case 'B':
						player.AddCapturedPiece(new Bishop(player.GetColor(), player.GetID()));
						break;
					case 'Q':
						player.AddCapturedPiece(new Queen(player.GetColor(), player.GetID()));
						break;
					case 'K':
						gameOver = true;
						break;
					}
					players[captureID]->RemovePiece({ p2.X, p2.Y });
					(*i)->MoveTo({ p2.X, p2.Y });
					board->Grid()[p1.Y][p1.X]->RemovePiece();
					board->Grid()[p1.Y][p1.X]->Vacant();
					board->Grid()[p2.Y][p2.X]->Place((*i)->GetSymbol(), player.GetColor());
					CheckKing(players, player, p2);
					validMove = true;
					break;
				}
				else
				{
					(*i)->MoveTo({ p2.X, p2.Y });
#if !defined(C_DEBUG) && defined(CB_DEBUG)
					if (player.GetID() == 0)
#ifndef C_DEBUG
						SendMove(BuildMoveBuffer(p1, p2, 6, 6));
#endif
					else
#ifdef CB_DEBUG
						SendMove(BuildMoveBuffer(p1, p2, 6, 6));
#endif
#endif
					board->Grid()[p1.Y][p1.X]->RemovePiece();
					board->Grid()[p1.Y][p1.X]->Vacant();
					board->Grid()[p2.Y][p2.X]->Place((*i)->GetSymbol(), (*i)->GetColor());
					CheckKing(players, player, p2);
					validMove = true;
					break;
				}
			}
		}
		break;
	default:
		return false;
		break;
	}
	if (validMove)
	{
		if (gameOver)
			player.Winner();
		return true;
	}
	else
		return false;
}


void Game::Turn() {
	if (players[0]->IsTurn())
	{
		window->PrintLine("Player 1, it's your turn.");
		Sleep(2000);
		window->ClearTextBuffer();
#ifdef RELEASE
		if (round <= 0)
		{
			window->PrintLine("Please enter a move command ");
			window->PrintLine("in algebraic notation.");
			Sleep(5000);
			window->ClearTextBuffer();
			window->PrintLine("For example, to move your pawn at A2 to A3,");
			window->PrintLine("type: \"PA2 A3\".");
			Sleep(5000);
		}
#endif
		std::string command;
		bool valid = true;
		do {
			window->ClearTextBuffer();
			window->PrintString("Player 1, enter your move command here: ");
			command = window->GetString();
			if (!MovePiece(*players[0], command))
			{
				window->ClearTextBuffer();
				window->PrintLine("Invalid move.");
				Sleep(2000);
				window->ClearTextBuffer();
				valid = false;
			}
			else
				valid = true;
		} while(valid == false);
		players[0]->SetTurn(false);
		players[1]->SetTurn(true);
	}
	else
	{
		std::string command;
		bool valid = true;
#ifndef C_DEBUG
		window->PrintLine("It's your opponent's turn.");
		Sleep(1000);
#endif
		window->ClearTextBuffer();
#if !defined(C_DEBUG) && !defined(CB_DEBUG)
		window->PrintLine("Please wait for your opponent's move.");
#endif
		do {
#if defined(C_DEBUG) || defined(CB_DEBUG)
			window->PrintLine("Player 2, it's your turn.");
			Sleep(1000);
			window->ClearTextBuffer();
			window->PrintString("Player 2, enter your move command here: ");
			command = window->GetString();
#endif
#if !defined(C_DEBUG) && !defined(CB_DEBUG)		
			ft.FT_Receive();
			command = RXBufferToCMD();
#endif
			if (!MovePiece(*players[1], command))
			{
#if defined(C_DEBUG) || defined(B_DEBUG) || defined(CB_DEBUG)
				window->ClearTextBuffer();
				window->PrintLine("Invalid Move");
#endif
#if !defined(C_DEBUG) && !defined(CB_DEBUG)
				SendErrorMove();
#endif
				valid = false;
			}
			else
				valid = true;
		} while (valid == false);
		players[1]->SetTurn(false);
		players[0]->SetTurn(true);	
	}
	round++;
}

void Game::Promote(Player& player, COORD pos) {
	bool valid = false;
	bool found = false;
	auto i = player.GetPieceList().begin();
	auto j = player.GetCapturedPieces().begin();
	for (; i != player.GetPieceList().end(); ++i) {
		if ((*i)->GetSymbol() == 'P' && (*i)->GetPosition().X == pos.X && (*i)->GetPosition().Y == pos.Y)
			break;
	}
	window->ClearTextBuffer();
	if (player.GetID() == 0)
	{
		window->PrintLine("Your pawn has reached the furthest rank.");
		Sleep(2000);
		window->ClearTextBuffer();
		do
		{
			Sleep(1000);
			window->ClearTextBuffer();
			window->PrintString("Please enter a piece type to promote your pawn to: ");
			std::string prom = window->GetString();
			window->ClearTextBuffer();
			switch (prom[0])
			{
			case 'P':
			case 'p':
				window->ClearTextBuffer();
				window->PrintString("You cannot promote your pawn to a pawn!");
				valid = false;
				break;
			case 'N':
			case 'n':
				for (; j != player.GetCapturedPieces().end(); ++j) {
					if ((*j)->GetSymbol() == 'N')
					{
						found = true;
						break;
					}
				}
				if (found)
				{
					player.AddPiece(new Knight(player.GetColor(), { pos.X, pos.Y }, player.GetID()));
					player.RemovePiece(*i);
					player.RemoveCapturedPiece(*j);
					board->Grid()[pos.Y][pos.X]->RemovePiece();
					board->Grid()[pos.Y][pos.X]->Place('N', player.GetColor());
					valid = true;
				}
				else
				{
					player.AddPiece(new Knight(player.GetColor(), { pos.X, pos.Y }, player.GetID()));
					player.RemovePiece(*i);
					board->Grid()[pos.Y][pos.X]->RemovePiece();
					board->Grid()[pos.Y][pos.X]->Place('N', player.GetColor());
					valid = true;
				}
				break;
			case 'B':
			case 'b':
				for (; j != player.GetCapturedPieces().end(); ++j) {
					if ((*j)->GetSymbol() == 'B')
					{
						found = true;
						break;
					}
				}
				if (found)
				{
					player.AddPiece(new Bishop(player.GetColor(), { pos.X, pos.Y }, player.GetID()));
					player.RemovePiece(*i);
					player.RemoveCapturedPiece(*j);
					board->Grid()[pos.Y][pos.X]->RemovePiece();
					board->Grid()[pos.Y][pos.X]->Place('B', player.GetColor());
					valid = true;
				}
				else
				{
					player.AddPiece(new Bishop(player.GetColor(), { pos.X, pos.Y }, player.GetID()));
					player.RemovePiece(*i);
					board->Grid()[pos.Y][pos.X]->RemovePiece();
					board->Grid()[pos.Y][pos.X]->Place('B', player.GetColor());
					valid = true;
				}
				break;
			case 'R':
			case 'r':
				for (; j != player.GetCapturedPieces().end(); ++j) {
					if ((*j)->GetSymbol() == 'R')
					{
						found = true;
						break;
					}
				}
				if (found)
				{
					player.AddPiece(new Rook(player.GetColor(), { pos.X, pos.Y }, player.GetID()));
					player.RemovePiece(*i);
					player.RemoveCapturedPiece(*j);
					board->Grid()[pos.Y][pos.X]->RemovePiece();
					board->Grid()[pos.Y][pos.X]->Place('R', player.GetColor());
					valid = true;
				}
				else
				{
					player.AddPiece(new Rook(player.GetColor(), { pos.X, pos.Y }, player.GetID()));
					player.RemovePiece(*i);
					board->Grid()[pos.Y][pos.X]->RemovePiece();
					board->Grid()[pos.Y][pos.X]->Place('R', player.GetColor());
					valid = true;
				}
				break;
			case 'Q':
			case 'q':
				for (; j != player.GetCapturedPieces().end(); ++j) {
					if ((*j)->GetSymbol() == 'Q')
					{
						found = true;
						break;
					}
				}
				if (found)
				{
					player.AddPiece(new Queen(player.GetColor(), { pos.X, pos.Y }, player.GetID()));
					player.RemovePiece(*i);
					player.RemoveCapturedPiece(*j);
					board->Grid()[pos.Y][pos.X]->RemovePiece();
					board->Grid()[pos.Y][pos.X]->Place('Q', player.GetColor());
					valid = true;
				}
				else
				{
					player.AddPiece(new Queen(player.GetColor(), { pos.X, pos.Y }, player.GetID()));
					player.RemovePiece(*i);
					board->Grid()[pos.Y][pos.X]->RemovePiece();
					board->Grid()[pos.Y][pos.X]->Place('Q', player.GetColor());
					valid = true;
				}
				break;
			case 'K':
			case 'k':
				window->PrintLine("You cannot have two kings!");
				valid = false;
				break;
			default:
				window->PrintLine("Invalid input");
				valid = false;
				break;
			}
		} while (valid == false);
	}
	else
	{
#if defined(C_DEBUG) || defined(CB_DEBUG)
		window->PrintLine("Your pawn has reached the furthest rank.");
		Sleep(2000);
		window->ClearTextBuffer();
#endif
		do
		{
			std::string prom;
#if defined(C_DEBUG) || defined(CB_DEBUG)
			Sleep(1000);
			window->ClearTextBuffer();
			window->PrintString("Please enter a piece type to promote your pawn to: ");
			prom = window->GetString();
			window->ClearTextBuffer();
#endif
#ifndef C_DEBUG
			if (!valid)
			{
				SendErrorMove();
			}

#ifndef CB_DEBUG
			ft.FT_Receive();
			prom = RXBufferToCMD();
#endif
#endif
			switch (prom[0])
			{
			case 'P':
			case 'p':
#if defined(C_DEBUG) || defined(CB_DEBUG)
				window->ClearTextBuffer();
				window->PrintString("You cannot promote your pawn to a pawn!");
#endif
				valid = false;
				break;
			case 'N':
			case 'n':
				for (; j != player.GetCapturedPieces().end(); ++j) {
					if ((*j)->GetSymbol() == 'N')
					{
						found = true;
						break;
					}
				}
				if (found)
				{
					player.AddPiece(new Knight(player.GetColor(), { pos.X, pos.Y }, player.GetID()));
					player.RemovePiece(*i);
					player.RemoveCapturedPiece(*j);
					board->Grid()[pos.Y][pos.X]->RemovePiece();
					board->Grid()[pos.Y][pos.X]->Place('N', player.GetColor());
					valid = true;
				}
				else
				{
					player.AddPiece(new Knight(player.GetColor(), { pos.X, pos.Y }, player.GetID()));
					player.RemovePiece(*i);
					board->Grid()[pos.Y][pos.X]->RemovePiece();
					board->Grid()[pos.Y][pos.X]->Place('N', player.GetColor());
					valid = true;
				}
				break;
			case 'B':
			case 'b':
				for (; j != player.GetCapturedPieces().end(); ++j) {
					if ((*j)->GetSymbol() == 'B')
					{
						found = true;
						break;
					}
				}
				if (found)
				{
					player.AddPiece(new Bishop(player.GetColor(), { pos.X, pos.Y }, player.GetID()));
					player.RemovePiece(*i);
					player.RemoveCapturedPiece(*j);
					board->Grid()[pos.Y][pos.X]->RemovePiece();
					board->Grid()[pos.Y][pos.X]->Place('B', player.GetColor());
					valid = true;
				}
				else
				{
					player.AddPiece(new Bishop(player.GetColor(), { pos.X, pos.Y }, player.GetID()));
					player.RemovePiece(*i);
					board->Grid()[pos.Y][pos.X]->RemovePiece();
					board->Grid()[pos.Y][pos.X]->Place('B', player.GetColor());
					valid = true;
				}
				break;
			case 'R':
			case 'r':
				for (; j != player.GetCapturedPieces().end(); ++j) {
					if ((*j)->GetSymbol() == 'R')
					{
						found = true;
						break;
					}
				}
				if (found)
				{
					player.AddPiece(new Rook(player.GetColor(), { pos.X, pos.Y }, player.GetID()));
					player.RemovePiece(*i);
					player.RemoveCapturedPiece(*j);
					board->Grid()[pos.Y][pos.X]->RemovePiece();
					board->Grid()[pos.Y][pos.X]->Place('R', player.GetColor());
					valid = true;
				}
				else
				{
					player.AddPiece(new Rook(player.GetColor(), { pos.X, pos.Y }, player.GetID()));
					player.RemovePiece(*i);
					board->Grid()[pos.Y][pos.X]->RemovePiece();
					board->Grid()[pos.Y][pos.X]->Place('R', player.GetColor());
					valid = true;
				}
				break;
			case 'Q':
			case 'q':
				for (; j != player.GetCapturedPieces().end(); ++j) {
					if ((*j)->GetSymbol() == 'Q')
					{
						found = true;
						break;
					}
				}
				if (found)
				{
					player.AddPiece(new Queen(player.GetColor(), { pos.X, pos.Y }, player.GetID()));
					player.RemovePiece(*i);
					player.RemoveCapturedPiece(*j);
					board->Grid()[pos.Y][pos.X]->RemovePiece();
					board->Grid()[pos.Y][pos.X]->Place('Q', player.GetColor());
					valid = true;
				}
				else
				{
					player.AddPiece(new Queen(player.GetColor(), { pos.X, pos.Y }, player.GetID()));
					player.RemovePiece(*i);
					board->Grid()[pos.Y][pos.X]->RemovePiece();
					board->Grid()[pos.Y][pos.X]->Place('Q', player.GetColor());
					valid = true;
				}
				break;
			case 'K':
			case 'k':
#ifdef C_DEBUG
				window->PrintLine("You cannot have two kings!");
#endif
				valid = false;
				break;
			default:
#ifdef C_DEBUG
				window->PrintLine("Invalid input");
#endif
				valid = false;
				break;
			}
		} while (valid == false);
	}
}