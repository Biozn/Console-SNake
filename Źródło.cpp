#include <iostream>
#include <ctime>
#include <Windows.h>
const int Width = 40, Height = 30;
enum { END = 0, UP, RIGHT, DOWN, LEFT };
void controls(int&);
void setup(wchar_t[][Width], int&, int&, int[], int[], int&, int&, int&, int&, int&, int&);
void logic(int&, int&, int&, int&, int&, int[], int[], int&, bool&, int&, int&, int&, int&);
void display(wchar_t[][Width], int&, int&, int&, int&, int[], int[], int&, int&, int&, int&, HANDLE&, DWORD&);
int main()
{
	SetConsoleTitle(TEXT("ASCI Snake 1.0"));
	//Console window size
	HWND console = GetConsoleWindow();
	RECT ConsoleRect;
	GetWindowRect(console, &ConsoleRect);
	MoveWindow(console, ConsoleRect.left, ConsoleRect.top, 325, 525, TRUE);
	//bufor konsoli txt
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWriten = 0;

	wchar_t screen[Height][Width];
	int xpos, ypos, hxpos, hypos, length, tailx[(Height - 2) * (Width - 3)], taily[(Height - 2) * (Width - 3)];
	int state, fxpos, fypos, delay, score = 1000;
	bool game = true;
	//seed for fruit position geeration
	srand(unsigned(time(NULL) + clock()));
	setup(screen, xpos, ypos, tailx, taily, length, state, fxpos, fypos, delay, score);
	while (game) {
		time_t start = clock();
		while (clock() - start < delay);
		logic(xpos, ypos, hxpos, hypos, length, tailx, taily, state, game, fxpos, fypos, score, delay);
		display(screen, xpos, ypos, hxpos, hypos, tailx, taily, length, fxpos, fypos, score,hConsole,dwBytesWriten);
	}
	CloseHandle(hConsole);
	std::cout.width(26);
	std::cout << std::endl << std::endl << std::endl << std::endl << std::endl << "\aKONIEC GRY" << std::endl;
	std::cout.width(28);
	std::cout << "TWOJ WYNIK TO: " << score << std::endl;
	time_t start = clock();
	while (clock() - start < 1000);
	std::cin.get();
	return 0;
}
void setup(wchar_t scr[][Width], int& xp, int& yp, int txp[], int typ[], int& len, int& state, int& fxpos, int& fypos, int& delay, int& score)
{
	xp = Width / 2;
	yp = Height / 2;
	len = 3;
	delay = 100;
	score = 0;
	int x, y;
	for (x = 0; x < len; ++x) {
		txp[x] = xp;
		typ[x] = yp + x + 1;
	}
	for (y = 0; y < Height; ++y) {
		for (x = 0; x < Width; ++x) {
			if (x == 0 && y != 0 || x == Width - 1 && y != 0 || y == 1 || y == Height - 1)
				scr[y][x] = L'#';
			else
				scr[y][x] = L' ';
		}
	}
	bool check = false;
	do {
		fxpos = rand() % (Width - 2) + 1;
		fypos = rand() % (Height - 3) + 2;
		for (x = 0; x < len; ++x) {
			if (txp[x] == fxpos && typ[x] == fypos) {
				check = true;
				break;
			}
		}
		if (check == true)
			continue;
	} while (fxpos == xp && fypos == yp);
	state = UP;
}
void display(wchar_t scr[][Width], int& xp, int& yp, int& hxp, int& hyp, int tpx[], int tpy[], int& length, int& fxpos, int& fypos, int& score, HANDLE& hConsole, DWORD& dwBytesWriten)
{
	scr[fypos][fxpos] = 0x1B1;
	scr[hyp][hxp] = L' ';
	scr[yp][xp] = L'@';
	for (int k = 0; k < length - 1; ++k) {
		scr[tpy[k]][tpx[k]] = L'@';
	}
	//one frame
	swprintf_s(&scr[0][Width - 12], 12, L"WYNIK: %4d", score);
	for (SHORT i = 0; i < SHORT(Height); ++i)
		WriteConsoleOutputCharacter(hConsole, scr[i], Width, { 0,i }, &dwBytesWriten);
}
void controls(int& st)
{
	if (GetKeyState(VK_RIGHT) & 0x8000 && st != LEFT)
		st = RIGHT;
	else if (GetKeyState(VK_LEFT) & 0x8000 && st != RIGHT)
		st = LEFT;
	else if (GetKeyState(VK_UP) & 0x8000 && st != DOWN)
		st = UP;
	else if (GetKeyState(VK_DOWN) & 0x8000 && st != UP)
		st = DOWN;
	else if (GetKeyState(VK_ESCAPE) & 0x8000)
		st = END;
}
void logic(int& xp, int& yp, int& hxp, int& hyp, int& len, int xtail[], int ytail[], int& state, bool& game, int& fxpos, int& fypos, int& score, int& delay)
{
	controls(state);
	//snake tail positions
	int xtmp = xtail[0], ytmp = ytail[0];
	xtail[0] = xp, ytail[0] = yp;
	int xtmp2, ytmp2, i;
	for (i = 1; i < len; ++i) {
		xtmp2 = xtail[i], ytmp2 = ytail[i];
		xtail[i] = xtmp, ytail[i] = ytmp;
		xtmp = xtmp2, ytmp = ytmp2;
	}
	//tail end
	hxp = xtail[i - 1];
	hyp = ytail[i - 1];
	//zmiana wspolrzednych weza wzgledem stanu kierunku
	switch (state) {
	case RIGHT:
		if (xp < Width - 2)
			++xp;
		else
			xp = 1;
		break;
	case LEFT:
		if (xp > 1)
			--xp;
		else
			xp = Width - 2;
		break;
	case UP:
		if (yp > 2)
			--yp;
		else
			yp = Height - 2;
		break;
	case DOWN:
		if (yp < Height - 2)
			++yp;
		else
			yp = 2;
		break;
	case END: game = false; break;
	}
	//fruit pick up
	if (xp == fxpos && yp == fypos) {
		++len;
		if (!((score += 10) % 100))
			delay -= 5;

		bool check = false;
		do {
			fxpos = rand() % (Width - 2) + 1;
			fypos = rand() % (Height - 3) + 2;
			for (i = 0; i < len; ++i) {
				if (fxpos == xtail[i] && fypos == ytail[i]) {
					check = true;
					break;
				}
			}
			if (check == true)
				continue;
		} while (fxpos == xp && fypos == yp);
	}
	//impact
	for (i = 0; i < len; ++i) {
		if (xp == xtail[i] && yp == ytail[i])
			game = false;
	}
}
