/*
	File: Window.h
	Author: Alex Turner
	Last Revision Date: 04/24/2020
*/


#pragma once
#include "ElectroWin.h"
#include "Board.h"
#include "Piece.h"

// Each space on the board is 6wx3h characters

/*
	The Window class serves as an interface between the win32 console API and the abstract Board class.
	Since the win32 console API's console character screen buffer data structure is represented by a 1-D array,
	and the Board class stores its character information in a 2-D array, this class is built around providing
	2-D array operation of the character grid.
*/
class Window : public Board
{
public:
	Window(int w, int h);
	~Window() { delete[] consoleBuffer; };

	// Write the contents of the screen buffer to the output handle
	void PrintBoard();

	// Initialize the screen buffer to a blank chess board
	void FillConsoleBuffer();

	// Clear the text buffer and board area, reinitialize the board area to a blank board and print to output
	void RefreshConsoleBuffer();

	// Clear and reinitialize the board area to a blank board and print to output (leave text buffer in current state)
	void RefreshBoard();

	// Set screen buffer elements in accordance to the current state of the Board class' grid
	void UpdateConsoleBuffer();

	// Pseudo message pump
	bool ProcessMessages();

	// Set screen buffer character at specified COORD position
	void SetChar(COORD pos, char c, WORD color);

	// Set screen buffer character at specified position
	void SetChar(short x, short y, char c, WORD color);

	// Set screen buffer character at specified position with an arbitrary board width parameter (deprecated)
	void SetChar(short x, short y, int w, char c, WORD color);


	// Sets cursor position in characters
	void SetCursor(COORD pos);
	
	// Sets cursor position to the top-left corner of the text buffer
	void ResetCursor();

	// Convert ascii code to corresponding character
	char ToAscii(int num) { return (char)(num + 48); };

	// Write string literal to screen buffer at cursor position
	void PrintString(const char* buf);
	
	// Write string to screen buffer at cursor position
	void PrintString(std::string str);
	
	// Write string to screen buffer at a specified position and place cursor on new line
	void PrintLine(std::string str, COORD pos);
	
	// Write string to screen buffer at cursor position and place cursor on new line
	void PrintLine(std::string str);
	
	// Write string literal to screen buffer at cursor position and place cursor on new line
	void PrintLine(const char*);
	
	// Clear single line of characters in the text buffer at specified position
	void ClearLine(int y);

	// Clear single line of characters in the text buffer at cursor position
	void ClearLine();

	// Clear entire text buffer and reset cursor position
	void ClearTextBuffer();

	// Place cursor on beginning of next line
	void NewLine();

	// Place cursor on beginning of current line
	void CarriageReturn();

	// Clear the left margin buffer
	void ClearLeftMarginBuffer();

	// Clear the bottom margin buffer
	void ClearBottomMarginBuffer();

	// Unused
	void FillExtraSpace();

	// Get a raw string from the console buffer
	char* GetBuffer();

	// Format raw input from the console so a std::string can be constructed from it
	std::string FormatString(char* buffer);

	// Get a std::string from the console buffer
	std::string GetString();



	// Setters and Getters //
	COORD& Cursor() { return cursor; };
	short& CursorX() { return cursor.X; };
	short& CursorY() { return cursor.Y; };
	HANDLE GetOutputHandle() const { return hOut; };
	HANDLE GetInputHandle() const { return hIn; };
	int GetWidth() const { return width; };
	int GetHeight() const { return height; };
	int GetConsoleBufferWidth() const { return consoleBufferWidth; };
	int GetConsoleBufferHeight() const { return consoleBufferHeight; };
	int GetLeftMarginBufferWidth() const { return leftMarginBufferW; };
	int GetLeftMarginBufferHeight() const { return leftMarginBufferH; };
	int GetBottomMarginBufferWidth() const { return bottomMarginBufferW; };
	int GetBottomMarginBufferHeight() const { return bottomMarginBufferH; };
	int GetTextBufferWidth() const { return textBufferW; };
	int GetTextBufferHeight() const { return textBufferH; };

private:
	// Console output handle for write calls
	HANDLE hOut;

	// Console input handle for read calls
	HANDLE hIn;

	// Window handle for changing the features of the console window
	HWND hWnd;

	// Width and height dimensions of the console window
	int width;
	int height;

	// Window rect for initializing the console window
	SMALL_RECT windowRect;

	// Dimensions of the board margins
	int leftMarginBufferW;
	int leftMarginBufferH;
	int bottomMarginBufferW;
	int bottomMarginBufferH;
	int textBufferW;
	int textBufferH;

	// Dimensions of the console screen buffer
	int consoleBufferWidth;
	int consoleBufferHeight;
	COORD bufferSize;
	
	// Stores information about each character in the screen buffer
	CHAR_INFO* consoleBuffer;

	// Used by the win32 console API to write to the console
	SMALL_RECT bufferRect;

	/*
		The text buffer is the portion of the screen buffer which IO between the player and the program is carried out.
		The text buffer displays prompts to the player, reads information from the player, 
		as well as relays information about actions performed by the opponent.
	*/

	// Rect that represents the outline of the text buffer.
	// It has character space dimensions of (width of board)x(height of space + 1)
	SMALL_RECT textBufferBox;

	// The left margin buffer is used to display the row numbers of the game board
	// It has character space dimensions of (width of space)x(height of board)
	SMALL_RECT leftMarginBufferBox;

	// The bottom margin buffer is used to display the file letters of the game board.
	// It has character space dimensions of (width of board)x(height of space)
	SMALL_RECT bottomMarginBufferBox;

	// Stores the character space coordinates of the position of cursor
	COORD cursor;
};