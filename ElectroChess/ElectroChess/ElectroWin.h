/*
	File: ElectroWin.h
	Author: Alex Turner
	Last Revision Date: 04/24/2020
*/

// Primary global definitions and includes here //

#pragma once

// Debug definitions:
// C_DEBUG is used to debug only the client application
// B_DEBUG is used to debug only the board
// CB_DEBUG is used to debug communications between the client application and the board
// RELEASE is used to run the application in its release configuration
#if 0
#define C_DEBUG
#endif
#if 0
#define B_DEBUG
#endif
#if 0
#define CB_DEBUG
#endif
#if 1
#define RELEASE
#endif

#define _WIN32_WINNT 0x0500 // support to Windows 2000; use _WIN32_WINNT_WIN10 0x0A00 for Windows 10
#include <Windows.h>
#include <vector>
#include <string>
#include <ctime>		   // So the player that goes first is chosen at random
#include <sstream>		   // Used to get inidvidual algebraic locations from a move command
#include <algorithm>	   // Used to remove elements of a std::vector in-place
#include <cctype>		   // Used to make translation of algebraic to square space coordinate commands easier via tolower()