#include "functions.h"

extern "C" {
#include "lualib/include/lua.h"
#include "lualib/include/lauxlib.h";
#include "lualib/include/lualib.h";
}

#ifdef _WIN32
#pragma comment(lib, "lualib/liblua53.a")
#endif

void printFile(std::string fileName) {
	console::ColorWrite(colors::lightblue, "[INFO]");
	std::cout << " Started reading " << fileName << "\n\n";

	std::ifstream file(fileName);
	unsigned int linePos = 0;
	std::string linePosStr;

	if (file.is_open() && file.good()) {
		std::string line;
		while (std::getline(file, line)) {
			linePos++;
			std::string color = "white";

			linePosStr = std::to_string(linePos);
			if (linePos < 10) {
				linePosStr = " " + std::to_string(linePos);
			}

			console::ColorWrite(colors::darkgray, linePosStr);

			std::cout << "  ";

			for (std::size_t i = 0; i < line.length(); ++i) { // for each index
				bool yellow = false;
				if (line[i] == '"') {
					if (line.substr(0, i).find("-") == std::string::npos) {
						if (color == "yellow") {
							yellow = true;
							console::SetColor(colors::white);
							color = "white";
						}
						else {
							console::SetColor(colors::yellow);
							color = "yellow";
						}
					}
				}
				else if (line[i] == '-') {
					console::SetColor(colors::green);
					color = "green";
				}
				else if (line[i] == '+' || line[i] == '-' || line[i] == '/' || line[i] == '*' || line[i] == '%') {
					if (line.substr(0, i).find("-") == std::string::npos) {
						std::string operation(1, line[i]);
						console::ColorWrite(colors::lightred, operation);
						continue;
					}
				}
				else if (line[i] == '.' && line[i + 1] == '.') {
					console::ColorWrite(colors::lightred, "..");
					i++;
					continue;
				}

				if (!yellow) {
					std::cout << line[i];
				}
				else {
					console::ColorWrite(colors::yellow, "\"");
					yellow = false;
				}
			}

			std::cout << "\n";
		}
	}
}

int printOverride(lua_State* L) {
	int nargs = lua_gettop(L);

	for (int i = 1; i <= nargs; ++i) {
		console::ColorWrite(colors::yellow, "[PRINT] ");
		std::cout << lua_tostring(L, i) << "\n";
	}

	return 0;
}

int warnOverride(lua_State* L) {
	int nargs = lua_gettop(L);

	for (int i = 1; i <= nargs; ++i) {
		console::ColorWrite(colors::magenta, "[WARN] ");
		std::cout << lua_tostring(L, i) << "\n";
	}

	return 0;
}

const struct luaL_Reg lib[] = {
	{"print", printOverride},
	{"warn", warnOverride}
};

std::string getFileName(const std::string path)  {
	std::string fileName = "";
	int index = -1;

	for (std::size_t i = 0; i < path.length(); ++i) {
		std::size_t found = path.find("\\", index + 1);

		if (found != std::string::npos) {
			index = i;
		}
	}

	return path.substr(index + 1, path.length() - index - 1);
}

int main() {
	std::string firstTitle = "C++ Lua Compiler";
	SetConsoleTitle(firstTitle.c_str());

	OPENFILENAME ofn;
	char szFileName[MAX_PATH] = "";
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFilter = "Supported script extension(*.lua)\0*.lua\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
	ofn.lpstrDefExt = "lua";

	if (!GetOpenFileName(&ofn)) {
		console::ColorWrite(colors::lightred, "[ERROR]");
		std::cout << " Couldn't get the file opened by explorer. \n";

		return 0;
	}

	std::string directory(szFileName);
	console::ColorWrite(colors::lightblue, "[INFO] ");
	std::string fileFromDir = getFileName(directory);
	std::cout << "Opened " + fileFromDir + " script. \n";
	SetConsoleTitle((firstTitle + " | " + fileFromDir).c_str());

	printFile(directory); 

	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	lua_getglobal(L, "_G");
	luaL_setfuncs(L, lib, 0);
	lua_pop(L, 1);

	std::cout << "\n";

	auto start = std::chrono::high_resolution_clock::now();
	int r = luaL_dofile(L, const_cast<char*>(directory.c_str()));
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsedTime = end - start;

	if (r != LUA_OK) {
		console::ColorWrite(colors::lightred, "\n[ERROR] ");
		std::cout << lua_tostring(L, -1) << "\n";
		SetConsoleTitle((firstTitle + " | " + fileFromDir + " | Error. | " + std::to_string(elapsedTime.count()) + " ms. |").c_str());
	}
	else {
		SetConsoleTitle((firstTitle + " | " + fileFromDir + " | Success. | " + std::to_string(elapsedTime.count()) + " ms. |").c_str());
	}

	lua_close(L);

	std::cin.get();
}