#include <iostream>
#include <ctime>
#include <Windows.h>
const int Width = 40, Height = 30;
enum { END = 0, UP, RIGHT, DOWN, LEFT };
void controls(int&);
void setup(wchar_t[][Width], int&, int&, int[], int[], int&, int&, int&, int&, int&, int&, bool&);
void logic(int&, int&, int&, int&, int&, int[], int[], int&, bool&, int&, int&, int&, int&);
void display(wchar_t[][Width], int&, int&, int&, int&, int[], int[], int&, int&, int&, int&, HANDLE&, DWORD&);
void choose(bool&, bool&);
void noise(int,HANDLE&,DWORD&);
int main()
{
	SetConsoleTitle(TEXT("ASCI Snake 1.0"));
	//Console window size
	HWND console = GetConsoleWindow();
	RECT ConsoleRect;
	/*GetWindowRect(console, &ConsoleRect);*/
	//MoveWindow(console, ConsoleRect.left, ConsoleRect.top, 325, 525, TRUE);	
	//bufor konsoli txt
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWriten = 0;
	COORD BuforSize;
	BuforSize.X = Width;
	BuforSize.Y = Height;

	CONSOLE_CURSOR_INFO lpCursor;
	lpCursor.bVisible = FALSE;
	lpCursor.dwSize = 50;
	SetConsoleCursorInfo(hConsole, &lpCursor);

	wchar_t screen[Height][Width];
	int xpos, ypos, hxpos, hypos, length, tailx[(Height - 2) * (Width - 3)], taily[(Height - 2) * (Width - 3)];
	int state, fxpos, fypos, delay, score;
	bool game, run=true;
	//seed for fruit position geeration
	srand(unsigned(time(NULL) + clock()));
	while (run) {
		setup(screen, xpos, ypos, tailx, taily, length, state, fxpos, fypos, delay, score, game);
		//game loop
		while (game) {
			/*SetConsoleCursorInfo(hConsole, &lpCursor);*/
			SetConsoleScreenBufferSize(hConsole, BuforSize);
			GetWindowRect(console, &ConsoleRect);
			MoveWindow(console, ConsoleRect.left, ConsoleRect.top, 330, 525, TRUE);
			time_t start = clock();
			while (clock() - start < delay);
			logic(xpos, ypos, hxpos, hypos, length, tailx, taily, state, game, fxpos, fypos, score, delay);
			display(screen, xpos, ypos, hxpos, hypos, tailx, taily, length, fxpos, fypos, score, hConsole, dwBytesWriten);
		}
		game = true;
		wchar_t endscreen[Height][Width];
		int x, y;
		for (y = 0; y < Height; ++y)
			for (x = 0; x < Width; ++x)
				endscreen[y][x] = L' ';
		swprintf_s(&endscreen[Height / 2 - 7][Width / 2 - 5], 11, L"KONIEC GRY");
		swprintf_s(&endscreen[Height / 2 - 6][Width / 2 - 5], 11, L"WYNIK:%4d", score);
		swprintf_s(&endscreen[Height / 2 - 5][Width / 2 - 6], 13, L"Jeszcze raz?");
		swprintf_s(&endscreen[Height / 2 - 4][Width / 2 - 9], 20, L"TAK[ENTER] NIE[ESC]");
		noise(500, hConsole, dwBytesWriten);
		//menu loop
		while (game) {
			GetWindowRect(console, &ConsoleRect);
			SetConsoleScreenBufferSize(hConsole, BuforSize);
			MoveWindow(console, ConsoleRect.left, ConsoleRect.top, 330, 530, TRUE);
			for (y = 0; y < Height; ++y)
				WriteConsoleOutputCharacter(hConsole, endscreen[y], Width, { 0,SHORT(y) }, &dwBytesWriten);
			choose(game, run);
		}
	}
	CloseHandle(hConsole);
	return 0;
}
void setup(wchar_t scr[][Width], int& xp, int& yp, int txp[], int typ[], int& len, int& state, int& fxpos, int& fypos, int& delay, int& score, bool& game)
{
	game = true;
	xp = Width / 2;
	yp = 3;
	len = 27;
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
	bool check;
	do {
		check = false;
		fxpos = rand() % (Width - 2) + 1;
		fypos = rand() % (Height - 3) + 2;
		for (x = 0; x < len; ++x) {
			if (txp[x] == fxpos && typ[x] == fypos) {
				check = true;
				break;
			}
		}
	} while ((fxpos == xp && fypos == yp) || check == true);
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
	//One frame
	swprintf_s(&scr[0][Width - 12], 12, L"WYNIK: %4d", score);
	for (int i = 0; i < Height; ++i)
		WriteConsoleOutputCharacter(hConsole, scr[i], Width, { 0,SHORT(i) }, &dwBytesWriten);
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
	//snake mowvement
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
		bool check;
		do {
			check = false;
			fxpos = rand() % (Width - 2) + 1;
			fypos = rand() % (Height - 3) + 2;
			for (i = 0; i < len; ++i) {
				if (fxpos == xtail[i] && fypos == ytail[i]) {
					check = true;
					break;
				}
			}
		} while ((fxpos == xp && fypos == yp) || check == true);
	}
	//snake impacts it self
	for (i = 0; i < len-1; ++i) {
		if (xp == xtail[i] && yp == ytail[i])
			game = false;
	}
}
void choose(bool& game, bool& run)
{
	if (GetKeyState(VK_RETURN) & 0x8000)
		game = false;
	else if (GetKeyState(VK_ESCAPE) & 0x8000) {
		game = false;
		run = false;
	}
}
void noise(int delay, HANDLE& console, DWORD& dwBytesWriten)
{
	const int ArrSize = 2;
	wchar_t scr[ArrSize][ArrSize][Width];
	int i,k;
	for (i = 0; i < Width; ++i) {
		scr[0][0][i] = L'#';
		scr[0][1][i] = L' ';
	}
	for (i = 0; i < Width; ++i) {
		scr[1][0][i] = L' ';
		scr[1][1][i] = L'@';
	}
	clock_t start = clock();
	while (clock() - start < delay) {
		/*clock_t begin = clock();
		while (clock() - begin < 33);*/

		k = 0;
		for (i = 0; i < Height; ++i)
			WriteConsoleOutputCharacter(console, scr[k++ % ArrSize][i % ArrSize], Width, { 0,SHORT(i) }, &dwBytesWriten);
		for (i = 0; i < Height; ++i)
			WriteConsoleOutputCharacter(console, scr[k % ArrSize][i % ArrSize], Width, { 0,SHORT(i) }, &dwBytesWriten);
	}
}
