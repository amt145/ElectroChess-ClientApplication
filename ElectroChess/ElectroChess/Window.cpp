#pragma once
#include "Window.h"

// Some friendly reminders
// SMALL_RECT==={LEFT,TOP,RIGHT,BOTTOM}
// SMALL_RECT==={top_left.x, top_left.y, bottom_right.x, bottom_right.y}

/*
namespace ConsoleForeground
{
	enum {
		BLACK = 0,
		DARKBLUE = FOREGROUND_BLUE,
		DARKGREEN = FOREGROUND_GREEN,
		DARKCYAN = FOREGROUND_GREEN | FOREGROUND_BLUE,
		DARKRED = FOREGROUND_RED,
		DARKMAGENTA = FOREGROUND_RED | FOREGROUND_BLUE,
		DARKYELLOW = FOREGROUND_RED | FOREGROUND_GREEN,
		DARKGRAY = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
		GRAY = FOREGROUND_INTENSITY,
		BLUE = FOREGROUND_INTENSITY | FOREGROUND_BLUE,
		GREEN = FOREGROUND_INTENSITY | FOREGROUND_GREEN,
		CYAN = FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE,
		RED = FOREGROUND_INTENSITY | FOREGROUND_RED,
		MAGENTA = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE,
		YELLOW = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,
		WHITE = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
	};
}
*/


Window::Window(int w, int h)
	:
	Board(w, h),
	width(GetBoardWidth() + GetSpaceWidth()),
	height(GetBoardHeight() + 2 * GetSpaceHeight()),
	windowRect({ short(0), short(0), short(width), short(height) }),
	leftMarginBufferH(GetBoardHeight()),
	leftMarginBufferW(GetSpaceWidth()),
	leftMarginBufferBox({ 0, 0, short(GetSpaceWidth()), short(GetBoardHeight()) }),
	bottomMarginBufferH(GetSpaceHeight()),
	bottomMarginBufferW(GetBoardWidth()),
	bottomMarginBufferBox({ short(GetSpaceWidth()), short(GetBoardHeight()), short(GetBoardWidth() + GetSpaceWidth()), short(GetBoardHeight() + GetSpaceHeight()) }),
	textBufferW(GetBoardWidth() + leftMarginBufferW),
	textBufferH(GetSpaceHeight() + 1),
	textBufferBox({ short(leftMarginBufferW), short(GetBoardHeight() + bottomMarginBufferH), short(GetBoardWidth() + leftMarginBufferW), short(GetBoardHeight() + bottomMarginBufferH + textBufferH) }),
	consoleBufferWidth(GetBoardWidth() + leftMarginBufferW),
	consoleBufferHeight(GetBoardHeight() + bottomMarginBufferH + textBufferH),
	bufferSize({ short(consoleBufferWidth), short(consoleBufferHeight) }),
	bufferRect({ short(0), short(0), bufferSize.X, bufferSize.Y }),
	cursor({ textBufferBox.Left, textBufferBox.Top })
{
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	hIn = GetStdHandle(STD_INPUT_HANDLE);
	hWnd = GetConsoleWindow();

	// Win32 console API structure used to initialize the screen buffer
	CONSOLE_SCREEN_BUFFER_INFO sbi;

	GetConsoleScreenBufferInfo(hOut, &sbi);
	
	// Set the screen buffer size to the maximum window size for easy expansion of the board (wasteful but simple)
	SetConsoleScreenBufferSize(hOut, { sbi.dwMaximumWindowSize.X, sbi.dwMaximumWindowSize.Y } );

	SetConsoleWindowInfo(hOut, true, &windowRect);
	
	// Used to force the console window to a specific size but was ultimately abandoned
	//SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX);

	// Dynamically allocate memory for the screen buffer with a length equivalent to the buffer's effective character space area
	consoleBuffer = new CHAR_INFO[(int)bufferSize.X * (int)bufferSize.Y];

	//textBuffer = new CHAR_INFO[GetBoardWidth() * textBufferH];
	//leftMarginBuffer = new CHAR_INFO[GetBoardHeight() * leftMarginBufferW];
	//bottomMarginBuffer = new CHAR_INFO[GetBoardWidth() * bottomMarginBufferH];

	SetConsoleTitle("Electro-Chess");

	// Remove the window's ugly scroll bar
	ShowScrollBar(hWnd, SB_BOTH, FALSE);

	// Initialize cursor position
	ResetCursor();

	// Initialize screen buffer
	RefreshConsoleBuffer();
}

void Window::SetCursor(COORD pos) {
	if (pos.X > textBufferBox.Right || pos.Y > textBufferBox.Bottom)
		cursor = { textBufferBox.Left, textBufferBox.Top };
	else
		cursor = { pos.X, pos.Y };
	SetConsoleCursorPosition(hOut, cursor);
}

void Window::ResetCursor() {
	SetCursor({ textBufferBox.Left, textBufferBox.Top });
}

void Window::SetChar(COORD pos, char c, WORD color) {
	consoleBuffer[int(pos.X) + consoleBufferWidth * int(pos.Y)].Attributes = color;
	consoleBuffer[int(pos.X) + consoleBufferWidth * int(pos.Y)].Char.AsciiChar = c;
}

void Window::SetChar(short x, short y, char c, WORD color) {
	consoleBuffer[x + consoleBufferWidth * y].Attributes = color;
	consoleBuffer[x + consoleBufferWidth * y].Char.AsciiChar = c;
}

void Window::SetChar(short x, short y, int w, char c, WORD color) {
	consoleBuffer[x + w*y].Attributes = color;
	consoleBuffer[x + w*y].Char.AsciiChar = c;
}

void Window::ClearLeftMarginBuffer() {
	WORD c = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
	int x = 2;
	int bH = GetBoardHeight();
	int sH = GetSpaceHeight();
	int val = 8;
	for (int y = 1; y < bH; y += sH)
		SetChar(x, y, ToAscii(val--), c);
}

void Window::ClearBottomMarginBuffer() {
	char letter = 65;
	WORD c = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
	int sW = GetSpaceWidth();
	int bW = GetBoardWidth();
	
	for (int y = GetBoardHeight(); y < GetBoardHeight() + bottomMarginBufferH; y++)
		for (int x = 0; x < GetBoardWidth() + GetSpaceWidth(); x++)
			SetChar(x, y, 0xdb, 0);
	
	int y = GetBoardHeight() + 1;
	for (int x = sW + 2; x < bW + sW; x += sW) {
		SetChar(x, y, letter++, c);
	}
}

void Window::FillExtraSpace() {
	for (int y = GetBoardHeight() + bottomMarginBufferH; y < GetBoardHeight() + bottomMarginBufferH + textBufferH; y++)
		for (int x = 0; x < GetSpaceWidth(); x++)
			SetChar(x, y, 178, FOREGROUND_INTENSITY);
}

void Window::RefreshConsoleBuffer() {
	FillConsoleBuffer();
	ClearTextBuffer();
	PrintBoard();
}
void Window::RefreshBoard() {
	FillConsoleBuffer();
	PrintBoard();
}

void Window::FillConsoleBuffer() {
	int spaceW = GetSpaceWidth();
	int spaceH = GetSpaceHeight();
	int h = GetBoardHeight();
	bool white = true;
	int startY = 1;
	int startX = 1;
	bool inLeftM = true;
	for (int y = startY; y < h + 1; y++) {
		for (int x = startX; x < consoleBufferWidth + 1; x += spaceW) {
			if (inLeftM)
			{
				for (int i = 0; i < spaceW; i++)
					SetChar(x - 1 + i, y - 1, 0xdb, 0);
				inLeftM = false;
			}
			else if (white && !inLeftM)
			{
				for (int i = 0; i < spaceW; i++)
					SetChar(x - 1 + i, y - 1, 0xff, colors[(int)COLOR::WHITE]);
				white = false;
			}
			else if (!white && !inLeftM)
			{
				for (int i = 0; i < spaceW; i++)
					SetChar(x - 1 + i, y - 1, 0xdb, colors[(int)COLOR::BLACK]);
				white = true;
			}
			if ((x - 1 + spaceW) % (consoleBufferWidth) == 0)
			{
				inLeftM = true;
				if (!(y % spaceH))
					white = white ? false : true;
			}
		}
	}
	ClearLeftMarginBuffer();
	ClearBottomMarginBuffer();
}

void Window::PrintBoard() {
	WriteConsoleOutput(hOut, consoleBuffer, bufferSize, { 0, 0 }, &bufferRect);
}

void Window::UpdateConsoleBuffer() {
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			SetChar(Grid()[i][j]->GetPiecePosition().X + leftMarginBufferW, Grid()[i][j]->GetPiecePosition().Y, Grid()[i][j]->GetPieceSymbol(), Grid()[i][j]->GetPieceColor());
		}
	}
}

// Windows-side console pseudo message pump
bool Window::ProcessMessages() {
	DWORD numEvents = 0;
	GetNumberOfConsoleInputEvents(hIn, &numEvents);
	if (numEvents != 0)
	{
		DWORD numEventsRead = 0;
		INPUT_RECORD* ir = new INPUT_RECORD[numEvents];
		ReadConsoleInput(hIn, ir, numEvents, &numEventsRead);
		/* 
		 * Tried to implement dynamic screen buffer refreshing, but the console window
		 * was created via the win32 console API, not the win32 API itself, so the
		 * standard locking console IO functions had to be used, instead of the win32
		 * API callbacks.
		 */
		/*
		MSG msg = {0};
		while (PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		switch (msg.message)
		{
		case WM_QUIT:
			return false;
			break;
		case WM_SIZE:
			// examine wParam for size event type
			// for dynamic resizing, newWidth=LO(lParam), newHeight=HI(lParam)
			//or newWidth = (lParam >> 16) & 0x0000ffff, newHeight = (lParam << 16) & 0xffff0000
			RefreshBoard();
			break;
		}
		*/
		for (DWORD i = 0; i < numEventsRead; i++) {
			switch (ir[i].EventType)
			{
			case KEY_EVENT:
				switch (ir[i].Event.KeyEvent.wVirtualKeyCode)
				{
				case VK_ESCAPE:
					delete[] ir;
					return false;
					break;
				}
				break;
			}
		}
		delete[] ir;
	}
	return true;
}


void Window::CarriageReturn() {
	SetCursor({ textBufferBox.Left, cursor.Y });
}

void Window::NewLine() {
	SetCursor({ textBufferBox.Left, short(cursor.Y + 1) });
}

void Window::ClearLine() {
	if (cursor.Y > textBufferBox.Bottom)
		ResetCursor();
	for (int x = textBufferBox.Left; x < textBufferBox.Right; x++)
		SetChar(x, cursor.Y, 0xdb, 0);
}

void Window::ClearLine(int y) {
	if (y > textBufferBox.Bottom)
		ResetCursor();
	for (int x = textBufferBox.Left; x < textBufferBox.Right; x++)
		SetChar(x, y, 0xdb, 0);
}

void Window::PrintString(const char* str) {
	if (cursor.X > short(textBufferW) || strlen(str) > textBufferW)
	{
		PrintString("string too long!");
		NewLine();
		return;
	}
	else if (cursor.Y > textBufferBox.Bottom)
		ClearTextBuffer();
	int len = strlen(str);
	for (int x = cursor.X, i = 0; i < len && x < textBufferBox.Right; x++, i++)
		SetChar(x, cursor.Y, (char)str[i], FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY | 0);
	SetCursor({ short(cursor.X + len), short(cursor.Y) });
	PrintBoard();
}

void Window::PrintString(std::string str) {
	if (cursor.X > short(textBufferW) || str.length() > textBufferW)
	{
		PrintString("string too long!");
		NewLine();
		return;
	}
	else if (cursor.Y >= short(textBufferBox.Top + textBufferH))
		ClearTextBuffer();
	for (int x = cursor.X, i = 0; i < str.length() && x < textBufferBox.Right; x++, i++)
		SetChar(x, cursor.Y, (char)str[i], FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY | 0);
	SetCursor({ short(cursor.X + str.length()), short(cursor.Y) });
	PrintBoard();
}

void Window::PrintLine(std::string str, COORD pos) {
	if (pos.X + str.length() > textBufferBox.Right)
	{
		PrintString("string too long!");
		NewLine();
		return;
	}
	else if (pos.Y > textBufferBox.Bottom)
	{
		PrintString("cannot put string there!");
		NewLine();
		return;
	}
	SetCursor({ pos.X, pos.Y });
	for (int y = pos.Y; y < pos.Y + 1; y++)
		for (int x = pos.X, i = 0; i < str.length() && x < textBufferBox.Right; x++, i++)
			SetChar(x, cursor.Y, (char)str[i], FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY | 0);
	SetCursor({ textBufferBox.Left, short(cursor.Y + 1) });
	PrintBoard();
}

void Window::PrintLine(std::string str) {
	if (cursor.X + str.length() > textBufferBox.Right)
	{
		PrintString("string too long!");
		NewLine();
		return;
	}
	else if (cursor.Y > textBufferBox.Bottom)
		ClearTextBuffer();
	for (int x = cursor.X, i = 0; i < str.length() && x < textBufferBox.Right; x++, i++)
		SetChar(x, cursor.Y, (char)str[i], FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY | 0);
	if (cursor.Y + 1 > textBufferBox.Bottom)
	{
		ResetCursor();
		ClearLine();
	}
	else
		SetCursor({ textBufferBox.Left, short(cursor.Y + 1) });
	PrintBoard();
}

void Window::PrintLine(const char* str) {
	if (cursor.X + strlen(str) > textBufferBox.Right)
	{
		PrintString("string too long!");
		NewLine();
		return;
	}
	else if (cursor.Y > textBufferBox.Bottom)
		ClearTextBuffer();
	for (int x = cursor.X, i = 0; i < strlen(str) && x < textBufferBox.Right; x++, i++)
		SetChar(x, cursor.Y, (char)str[i], FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY | 0);
	if (cursor.Y + 1 > textBufferBox.Bottom)
	{
		ResetCursor();
		ClearLine();
	}
	else
		SetCursor({ textBufferBox.Left, short(cursor.Y + 1) });
	PrintBoard();
}

std::string Window::FormatString(char* buf) {
	std::string str = "";
	for (int i = 0; i < strlen(buf); i++) {
		if (buf[i] == '\r')
			break;
		else
			str.push_back(buf[i]);
	}
	return str;
}

std::string Window::GetString() {
	DWORD length = 0, numRead = 0;
	char buf[1024];
	length = (DWORD)strlen(buf);
	ReadConsole(hIn, buf, length, &numRead, NULL);
	std::string str = std::string(FormatString(buf));
	return str;
}

char* Window::GetBuffer() {
	DWORD length = 0, numRead = 0;
	char buf[1024];
	length = (DWORD)strlen(buf);
	ReadConsole(hIn, buf, length, &numRead, NULL);
	return buf;
}

void Window::ClearTextBuffer() {
	for (int y = textBufferBox.Top; y < textBufferBox.Bottom; y++)
		for (int x = 0; x < textBufferBox.Right; x++)
			SetChar(x, y, 0xdb, 0);
	ResetCursor();
}