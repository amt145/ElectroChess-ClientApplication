#pragma once
#include "ElectroWin.h"
#include "Vec2.h"
#include "Rect.h"
#include "Board.h"
#include "Piece.h"

//each space on the board is 6wx3h characters

class Window : public Board
{
public:
	Window(int w, int h);
	~Window() { delete[] consoleBuffer; delete[] textBuffer; delete[] leftMarginBuffer; delete[] bottomMarginBuffer; };
	void PrintBoard();
	//void PlacePiece(Piece piece, COORD pos);
	void FillConsoleBuffer();
	void RefreshConsoleBuffer();
	void RefreshBoard();
	void UpdateConsoleBuffer();
	bool ProcessMessages();
	void SetChar(COORD pos, char c, WORD color);
	void SetChar(short x, short y, char c, WORD color);
	void SetChar(short x, short y, int w, char c, WORD color);


	// set cursor position in characters
	void SetCursor(COORD pos);
	void ResetCursor();

	char ToAscii(int num) { return (char)(num + 48); };

	void PrintString(const char* buf);
	void PrintString(std::string str);
	void PrintLine(std::string str, COORD pos);
	void PrintLine(std::string str);
	void PrintLine(const char*);
	void ClearLine(int y);
	void ClearLine();
	void ClearTextBuffer();

	void NewLine();
	void CarriageReturn();

	void ClearLeftMarginBuffer();
	void ClearBottomMarginBuffer();

	void FillExtraSpace();

	char* GetBuffer();
	std::string FormatString(char* buffer);
	std::string GetString();

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
	HANDLE hOut;
	HANDLE hIn;
	HWND hWnd;
	int width;
	int height;

	SMALL_RECT windowRect;

	int leftMarginBufferW;
	int leftMarginBufferH;
	int bottomMarginBufferW;
	int bottomMarginBufferH;
	int textBufferW;
	int textBufferH;
	int consoleBufferWidth;
	int consoleBufferHeight;

	COORD bufferSize;
	CHAR_INFO* consoleBuffer;
	SMALL_RECT bufferRect;

	SMALL_RECT textBufferBox;
	CHAR_INFO* textBuffer;

	SMALL_RECT leftMarginBufferBox;
	CHAR_INFO* leftMarginBuffer;
	SMALL_RECT bottomMarginBufferBox;
	CHAR_INFO* bottomMarginBuffer;

	COORD cursor;
};