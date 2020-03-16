#include <iostream>
#include <iostream>
#include <Windows.h>
#include <cstdbool>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <locale>
#include <ctime>
#include <cstdlib>
#include <cmath>

#define FIELD_HEIGHT 40
#define FIELD_WIDTH 100

wchar_t target = '^';


wchar_t frontBuffer[FIELD_HEIGHT][FIELD_WIDTH];
wchar_t backBuffer[FIELD_HEIGHT][FIELD_WIDTH];

void MoveCursorto(const int x, const int y)
{
	const COORD pos = { x, FIELD_HEIGHT - y - 1 };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void CursorHide(char show)
{
	HANDLE hConsole;
	CONSOLE_CURSOR_INFO ConsoleCursor;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	ConsoleCursor.bVisible = show;
	ConsoleCursor.dwSize = 1;
	SetConsoleCursorInfo(hConsole, &ConsoleCursor);
}

void initMap()
{
	for (int i = 0; i < FIELD_HEIGHT; i++)
		for (int j = 0; j < FIELD_WIDTH; j++)
		{
			frontBuffer[i][j] = L' ';
			backBuffer[i][j] = L' ';
		}
}

void render()
{
	for (int j = 0; j < FIELD_HEIGHT; j++)
		for (int i = 0; i < FIELD_WIDTH; i++)
		{
			if (backBuffer[j][i] != frontBuffer[j][i])
			{
				MoveCursorto(i, j);
				if (backBuffer[j][i] == L' ')
					std::wcout << L" ";
				else
					std::wcout << backBuffer[j][i];
			}
		}
	for (int j = 0; j < FIELD_HEIGHT; j++)
		for (int i = 0; i < FIELD_WIDTH; i++)
		{
			frontBuffer[j][i] = backBuffer[j][i];
			backBuffer[j][i] = L' ';
		}
}

void drawToBackBuffer(const int x, const int y, const wchar_t* k)
{
	MoveCursorto(x, y);
	int ix = 0;
	while (1)
	{
		if (k[ix] == L'\0')
			break;
		backBuffer[y][x + ix] = k[ix];
		ix++;
	}
}



//1. 단계 확인 및 방향설정
//2. 움직이기
//3. 단계 변화 시키기

typedef enum direction : int {
	up = 0,
	upright = 1,
	right = 2,
	rightdown = 3,
	down = 4,
	downleft = 5,
	left = 6,
	leftup = 7,
	lastDirection = 8,
	stay = 9,
}direction;

direction opposeDirectionOf(direction d)
{
	switch (d)
	{
	case up:
		return down;
	case upright:
		return downleft;
	case right:
		return left;
	case rightdown:
		return leftup;
	case down:
		return up;
	case downleft:
		return upright;
	case left:
		return right;
	case leftup:
		return rightdown;
	default:
		return lastDirection;
	}
}
direction clockwiseTurnFrom(direction dir)
{
	switch (dir)
	{
	case up:
		return upright;
	case upright:
		return right;
	case right:
		return rightdown;
	case rightdown:
		return down;
	case down:
		return downleft;
	case downleft:
		return left;
	case leftup:
		return up;
	default:
		return lastDirection;
	}
}

int Int(direction dir)
{
	switch (dir)
	{
	case up:
		return 0;
	case upright:
		return 1;
	case right:
		return 2;
	case rightdown:
		return 3;
	case down:
		return 4;
	case downleft:
		return 5;
	case left:
		return 6;
	case leftup:
		return 7;
	case lastDirection:
		return 8;
	}
}

template <typename T>
class Stack
{
private:
	int capacity;
	T* store = NULL;
	int top = -1;
public:
	Stack(int cap)
	{
		capacity = cap;
		store = new T[capacity];
	}
	~Stack()
	{
		delete[] store;
	}
	void push(T data)
	{
		if (top >= capacity - 1)
			exit(0);
		store[++top] = data;
	}
	T getTopData()
	{
		return store[top];
	}
	T getOri()
	{
		if (top < 1)
			return store[top];
		return store[top - 1];
	}
	T pop()
	{
		if (top < 0)
			exit(0);
		return store[top--];
	}
	int getTop()
	{
		return top;
	}
};



class Pos
{
public:
	int x;
	int y;
	Pos()
	{
		x = -1;
		y = -1;
	}
	Pos(int x, int y)
	{
		this->x = x;
		this->y = y;
	}
	Pos& operator=(const Pos& rhs)
	{
		if (this == &rhs)
			return *this;
		else
		{
			this->x = rhs.x;
			this->y = rhs.y;
			return *this;
		}
	}

	Pos& operator+=(const Pos& rhs)
	{
		this->x = this->x + rhs.x;
		this->y = this->y + rhs.y;
		return *this;
	}

	Pos& operator-=(const Pos& rhs)
	{
		this->x = this->x - rhs.x;
		this->y = this->y - rhs.y;
		return *this;
	}

	const Pos operator+(const Pos& other) const
	{
		return Pos(*this) += other;
	}

	const Pos operator +(direction dir) const
	{
		switch (dir)
		{
		case up:
			return *this + Pos(0, +1);
		case upright:
			return *this + Pos(+1, +1);
		case right:
			return *this + Pos(+1, 0);
		case rightdown:
			return *this + Pos(+1, -1);
		case down:
			return *this + Pos(0, -1);
		case downleft:
			return *this + Pos(-1, -1);
		case left:
			return *this + Pos(-1, 0);
		case leftup:
			return *this + Pos(-1, 1);
		default:
			return *this;
		}
	}

	const Pos operator-(const Pos& other) const
	{
		return Pos(*this) -= other;
	}

	const Pos operator -(direction dir) const
	{
		switch (dir)
		{
		case up:
			return *this - Pos(0, +1);
		case upright:
			return *this - Pos(+1, +1);
		case right:
			return *this - Pos(+1, 0);
		case rightdown:
			return *this - Pos(+1, -1);
		case down:
			return *this - Pos(0, -1);
		case downleft:
			return *this - Pos(-1, -1);
		case left:
			return *this - Pos(-1, 0);
		case leftup:
			return *this - Pos(-1, 1);
		default:
			return *this;
		}
	}

};

bool checkObj(const wchar_t obj, Pos p)
{
	if (backBuffer[p.y][p.x] == obj)
		return true;
	else
		return false;
}

std::ostream& operator<<(std::ostream& os, const Pos& p)
{
	os << "(x:" << p.x << "," << "y:" << p.y << ")";
	return os;
}
std::ostream& operator<<(std::ostream& os, const direction dir)
{
	switch (dir)
	{
	case left:
		os << "left";
		break;
	case right:
		os << "right";
		break;
	case up:
		os << "up";
		break;
	case down:
		os << "down";
		break;
	default:
		os << "stay";
		break;
	}
	return os;
}

class Enemy
{
private:
	const wchar_t* image = L"%";  
	int attackRange = 3;
	Pos head = pos + Pos(0, 1);
	Stack<direction> escaping;
	bool escape[3] = { false, false, false };
	direction prev[2] = { lastDirection, lastDirection };
	
public:
	Pos pos;	
	Enemy(Pos p) : escaping(10)
	{	
		pos = p;
	}

	bool canShootPlayer()
	{
		int attackRange_ = (int)(attackRange / sqrt(2.0));
		bool wallFound[8] = { false, false, false, false, false, false, false, false };
		bool playerFound = false;
		int i, j;
		direction searchPath;
		for (i = 0; i < 8; i++)
		{

			Pos searchNode = pos;
			switch (i)
			{
			case 0:
				searchPath = up;
				break;
			case 1:
				searchPath = upright;
				break;
			case 2:
				searchPath = right;
				break;
			case 3:
				searchPath = rightdown;
				break;
			case 4:
				searchPath = down;
				break;
			case 5:
				searchPath = downleft;
				break;
			case 6:
				searchPath = left;
				break;
			case 7:
				searchPath = leftup;
				break;
			default:
				searchPath = lastDirection;
				break;
			}
			if (i % 2 == 0)
			{
				for (j = 0; j < attackRange; j++) {
					searchNode = searchNode + searchPath;
					playerFound = checkObj(L'^', searchNode);
					wallFound[i] = checkObj(L'|', searchNode) || checkObj(L'-', searchNode);
					if (playerFound || wallFound[i])
						break;
				}
			}
			else
			{
				for (j = 0; j < attackRange_; j++) {
					searchNode = searchNode + searchPath;
					playerFound = checkObj(L'^', searchNode);
					wallFound[i] = checkObj(L'|', searchNode) || checkObj(L'-', searchNode);
					if (playerFound || wallFound[i])
						break;
				}
			}
			if (playerFound) {
				head = pos + searchPath;
				break;
			}
		}
		return playerFound;
	}

	bool canMoveTo(direction des)
	{
		return !(checkObj(L'%', pos + des) || checkObj(L'-', pos + des) || checkObj(L'^', pos + des) || checkObj(L'|', pos + des));
	}

	
	// ori 내가 가려던 방향.
	direction turn(direction ori,direction toX, direction toY)
	{
		Pos l = pos, r = pos;
		int found = -1;
		bool inc[2] = { true, true };
		direction toX_ = toX, toY_ = toY;
		if (toX == lastDirection) toX_ = right;
		if (toY == lastDirection) toY_ = up;
		while (found == -1)
		{
			if (ori == left || ori == right) // turn
			{
				if (inc[0])
					l = l + toY_;
				if (checkObj(L'-', l))
					inc[0] = false;
				if (inc[0] && checkObj(L' ', l + ori))
					found = 0;

				if (inc[1])
					r = r + opposeDirectionOf(toY_);
				if (checkObj(L'-', r))
				{
					inc[1] = false;
				}
				if (found == -1 && inc[1] && checkObj(L' ', r + ori))
					found = 1;
			}
			else if (ori == up || ori == down)
			{
				if (inc[0])
					l = l + toX_;
				if (checkObj(L'|', l))
				{
					inc[0] = false;
				}
				if (inc[0] && checkObj(L' ', l + ori))
					found = 0;
				
				if (inc[1])
					r = r + opposeDirectionOf(toX_);
				if (checkObj(L'|', r))
				{
					inc[1] = false;
				}
				if (found == -1 && inc[1] && checkObj(L' ', r + ori))
					found = 1;
				
			}
			/*if (!inc[0] && !inc[1])
				return stay;*/
		}
		if (ori == left || ori == right)
		{
			if (found == 0) return toY_;
			else if (found == 1) return opposeDirectionOf(toY_);
		}
		else if (ori == up || ori == down)
		{
			if (found == 0) return toX_;
			else if (found == 1) return opposeDirectionOf(toX_);
		}
	}
	void move()
	{
		direction toX, toY;
		direction moveTo;
		// 나중에 똑똑하게 쏠자리로 이동하는 걸 추가할 예정. toY, toX 가 가장 가까운 쏠자리로 가는 걸로 바꿀거임.

		if (pos.x > 10)
			toX = left;
		else if (pos.x < 10)
			toX = right;
		else
			toX = lastDirection;

		if (pos.y > 10)
			toY = down;
		else if (pos.y < 10)
			toY = up;
		else
			toY = lastDirection;

		if (canShootPlayer())
			return;
		if (escaping.getTop() == -1) // empty : free.
		{
			if (toY != lastDirection && canMoveTo(toY))
			{
				pos = pos + toY;
				prev[0] = toY;
			}
			else if (toX != lastDirection && canMoveTo(toX))
			{
				pos = pos + toX;
				prev[0] = toX;
			}
			else
			{
				if (toY != lastDirection)
				{
					moveTo = toY;
					escaping.push(moveTo);
					while (!canMoveTo(moveTo))
					{
						moveTo = turn(moveTo, toX, toY);
						escaping.push(moveTo);
					}
					pos = pos + moveTo;
					if (canMoveTo(escaping.getOri()))
						escaping.pop();// top.clear
				}
				else
				{
					if (toX != lastDirection) // toX 불가.
					{
						moveTo = toX;
						escaping.push(moveTo);					
						while (!canMoveTo(moveTo))
						{
							moveTo = turn(moveTo, toX, toY);
							escaping.push(moveTo);
						}

						pos = pos + moveTo;
						if (canMoveTo(escaping.getOri()))
							escaping.pop();// top.clear
						return;
					}
				}
			}
			/*else if (toX != lastDirection && canMoveTo(toX))
				pos = pos + toX;*/

		}
		else //escaping.
		{
			if (canMoveTo(escaping.getTopData()))
			{
				moveTo = escaping.getTopData();
				pos = pos + escaping.getTopData();
				if (canMoveTo(escaping.getOri()))
					escaping.pop();// top.clear

			}
			else
			{
				moveTo = escaping.getTopData();
				while (!canMoveTo(moveTo))
				{
					moveTo = turn(moveTo, toX, toY);
					escaping.push(moveTo);
				}
				pos = pos + moveTo;
				direction test = escaping.getOri();
				bool tests = canMoveTo(escaping.getOri());
				if (canMoveTo(escaping.getOri()))
					escaping.pop();// top.clear
			}
		}
	}


void drawEnemy()
{
	drawToBackBuffer(pos.x, pos.y, image);
}

	// 가까운 벽으로 먼저 붙고
	// 그담음 그벽을 탈출하는식. 이때 prev 밯향으로는 가지 않는다.
};



void drawMap()
{
	for (int i = 0; i < FIELD_HEIGHT; i++)
	{
		drawToBackBuffer(0, i, L"|");
	}
	for (int i = 0; i < FIELD_HEIGHT; i++)
	{
		drawToBackBuffer(FIELD_WIDTH - 1, i, L"|");
	}
	for (int i = 1; i < FIELD_WIDTH; i++)
	{
		drawToBackBuffer(i, 0, L"-");
	}
	for (int i = 1; i < FIELD_WIDTH; i++)
	{
		drawToBackBuffer(i, FIELD_HEIGHT - 1, L"-");
	}
}

int main()
{
	CursorHide(0);
	initMap();
	Enemy e1(Pos(30, 30)), e2(Pos(3, 30)), e3(Pos(1, 30)), e4(Pos(30, 1));
	while (1)
	{
		backBuffer[10][10] = L'^';
		backBuffer[10][12] = L'|';
		drawMap();
		e1.drawEnemy();
		e2.drawEnemy();
		e3.drawEnemy();
		e4.drawEnemy();
		e1.move();
		/*
		e4.move();
		e3.move();
		e2.move();
		*/
		render();
		Sleep(100);
	}
	return 0;
}

// 프로그램 실행: <Ctrl+F5> 또는 [디버그] > [디버깅하지 않고 시작] 메뉴
// 프로그램 디버그: <F5> 키 또는 [디버그] > [디버깅 시작] 메뉴

// 시작을 위한 팁: 
//   1. [솔루션 탐색기] 창을 사용하여 파일을 추가/관리합니다.
//   2. [팀 탐색기] 창을 사용하여 소스 제어에 연결합니다.
//   3. [출력] 창을 사용하여 빌드 출력 및 기타 메시지를 확인합니다.
//   4. [오류 목록] 창을 사용하여 오류를 봅니다.
//   5. [프로젝트] > [새 항목 추가]로 이동하여 새 코드 파일을 만들거나, [프로젝트] > [기존 항목 추가]로 이동하여 기존 코드 파일을 프로젝트에 추가합니다.
//   6. 나중에 이 프로젝트를 다시 열려면 [파일] > [열기] > [프로젝트]로 이동하고 .sln 파일을 선택합니다.
