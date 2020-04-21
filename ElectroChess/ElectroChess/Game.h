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
	// Game object constructors
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
	
	// Initialize Game object within constructor
	void Init();

	// Initialize win32 serial port
	void InitComms();

	// Write to serial port
	void BufferPut(uint8_t c);

	// Read serial port
	uint8_t BufferGet();

	bool BufferEmpty() { return bufEmpty; };
	
	// Translate FT buffer to character command to be passed into MovePiece
	std::string RXBufferToCMD();
	
	// Construct an FT buffer and send over serial port using Fast Transfer protocol
	void SendMove(int16_t* moveBuffer);

	// Translates a particular move into an FT buffer
	int16_t* BuildMoveBuffer(COORD src, COORD dest, int ts, int td);
	
	// Send move consisting of impossible sentinel values to indicate something went wrong
	void SendErrorMove();

	// En Passant move is handled in a separate function since it must be checked and carried out more than once
	bool EnPassant(std::vector<Player*>& players, Player& player, COORD p1, COORD p2);

	// Checks and updates the status of the moving player's king at the end of the move
	void CheckKing(std::vector<Player*>& players, Player& player, COORD dest);

	// Initialize player objects
	void InitPlayers();

	// Update the game's structures at the end of a round
	void Update();

	// Use the window member object to update the board
	void UpdateBoard();

	// The pawn promotion process is complicated so the procedure is handled in a function
	void Promote(Player& player, COORD pos);

	// Main game loop
	int Run();

	// Wraps the concept of a round of chess in a separate function. It is used to 
	// start the round by receiving player input and finish up the round by updating
	// game member objects
	void Turn();

	// Translates a chess move in algebraic notation to a set of square space coordinates
	COORD TranslateCommandCoords(std::string str);

	// Primary function to move a player's piece given a chess move in algebraic notation
	bool MovePiece(Player& player, std::string command);

	// Rudimentary pause function
	void Pause() { int i = getchar(); };
private:
	// Fast Transfer structure for communication between the board and client application
	FastTransfer ft;

	// Used to send a move to the board in a single line
	int16_t moveBuffer[MOVE_SZ];

	// Communications handle
	HANDLE hComm;

	// Used in the embedded Fast Transfer class
	DWORD bytes;
	bool bufEmpty;

	// Round number
	int round;

	// End game condition
	bool gameOver;

	// Board class object
	Board* board;

	// Window class object
	Window* window;

	// List of player objects
	std::vector<Player*> players;
};