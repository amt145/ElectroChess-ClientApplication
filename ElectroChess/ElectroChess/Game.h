#pragma once
#include "ElectroWin.h"

//#include "../FastTransferPlusPlus/ft.h"
#include <stdbool.h>
#include <stdint.h>
#include "../FastTransferPlusPlus/ft_config.h"
#include "../FastTransferPlusPlus/buffer.h"
#include "../FastTransferPlusPlus/parser.h"
#include "../FastTransferPlusPlus/crc.h"

#include "Window.h"
#include "Board.h"
#include "Player.h"
#include "Pawn.h"
#include "Rook.h"
#include "Bishop.h"
#include "Knight.h"
#include "Queen.h"
#include "King.h"

#define MOVE_SZ 7

class Game
{
private:
	class FastTransfer
	{
	public:
		///--------------------------------------------------------------------------
		/// FT_t is an alias for a type representing a single fast-transfer handle.
		///--------------------------------------------------------------------------

		////////////////////////////////////////////////////////////////////////////////
		//                                                                            //
		//                            FastTransfer Methods                            //
		//                                                                            //
		////////////////////////////////////////////////////////////////////////////////

		FastTransfer(Game* game, uint8_t address);
		///--------------------------------------------------------------------------
		/// Initializes the FastTransfer instance associated with the given handle
		/// by setting all values of the arr to 0 and all modification flags
		/// to TRUE.  Also connects the instance to serial transmit and receive
		/// buffers via the three callback functions.
		///
		/// Parameters:
		///   - handle:  Memory address of a handle to a FastTransfer instance.
		///   - address: FastTransfer address of the node.
		///   - f_put:   Callback function that adds a byte to a transmit buffer.
		///   - f_get:   Callback function that reads a byte from a receive buffer.
		///   - f_empty: Callback function that returns true iff receive buffer
		///              contains 0 bytes.
		///--------------------------------------------------------------------------

		int16_t FT_Read(uint8_t index);
		///--------------------------------------------------------------------------
		/// Read a single value from the local fast-transfer arr.
		/// As a side-effect, this function unsets the modification flag for the
		/// element at the given index.
		///--------------------------------------------------------------------------

		bool FT_Modified(uint8_t index);
		///--------------------------------------------------------------------------
		/// Read the modification flag, which is TRUE if and only if the data at the
		/// given index has not already been read.
		///--------------------------------------------------------------------------

		void FT_Receive();
		///--------------------------------------------------------------------------
		/// Reads the bytes from the serial buffer, parses them, and updates the
		/// local arr if any valid messages were received.
		///--------------------------------------------------------------------------

		void FT_ToSend(uint8_t index, int16_t data);
		///--------------------------------------------------------------------------
		/// Adds the given data to the given handle's next packet to be transmitted.
		///--------------------------------------------------------------------------

		void FT_Send(uint8_t address);
		///--------------------------------------------------------------------------
		/// Transmits the handle's packet to the given address.
		///--------------------------------------------------------------------------

		Buffer_t& GetRXBuffer() { return receive_buf; }
		Buffer_t& GetTXBuffer() { return transmit_buf; }
		uint16_t* GetBuffer() { return arr; };
	private:
		// local data:
		uint8_t  address;
		uint16_t arr[ARRAY_SZ];
		bool     flags[ARRAY_SZ];
		// buffers:
		Buffer_t receive_buf;
		Buffer_t transmit_buf;
		Game* game;
	};

public:
	Game(int w, int h);
	Game(int dim);
	Game();
	~Game() {
		delete board;
		delete window;
		for (size_t i = 0; i < players.size(); i++) {
			delete players[i];
		}
		CloseHandle(hComm);
	};
	HANDLE GetWindowOutputHandle() const;
	void Init();

	void InitComms();
	void BufferPut(uint8_t c);
	uint8_t BufferGet();
	bool BufferEmpty() { return bufEmpty; };
	std::string RXBufferToCMD();
	
	void SendMove(int16_t* moveBuffer);
	int16_t* BuildMoveBuffer(COORD src, COORD dest, int ts, int td);
	void SendErrorMove();

	bool EnPassant(std::vector<Player*>& players, Player& player, COORD p1, COORD p2);
	//void CheckKing(std::vector<Piece*>::iterator& playerPieces, std::vector<Piece*>::iterator& otherPieces, std::vector<COORD>::iterator& capturePattern, COORD otherKCoord);

	void CheckKing(std::vector<Player*>& players, Player& player, COORD dest);

	void InitPlayers();
	void Update();
	void UpdateBoard();
	void Promote(Player& player, COORD pos);
	int Run();
	void Turn();
	COORD TranslateCommandCoords(std::string str);
	bool MovePiece(Player& player, std::string command);
	void Pause() { int i = getchar(); };
private:
	FastTransfer ft;
	int16_t moveBuffer[MOVE_SZ];

	HANDLE hComm;
	DWORD bytes;
	bool bufEmpty;
	int round;
	bool gameOver;
	Board* board;
	Window* window;
	std::vector<Player*> players;
	std::vector<Piece> piecesInPlay;
};