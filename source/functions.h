#pragma once

#include <Windows.h>
#include <string>
#include <iostream>
#include <fstream>
#include <chrono>

enum colors{blue = 1, green, cyan, red, magenta, brown, lightgray, darkgray, lightblue, lightgreen, lightcyan, lightred, lightmagenta, yellow, white};

namespace console {
	void SetColor(colors color) {
		HANDLE stdHANDLE = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(stdHANDLE, color);
	}

	void ColorWrite(colors color, std::string text) {
		HANDLE stdHANDLE = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(stdHANDLE, color);
		std::cout << text;
		SetConsoleTextAttribute(stdHANDLE, colors::white);
	}
}