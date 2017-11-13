#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <stack>
#include <set>
#include <queue>
#include <algorithm>
#include <ctime>
#include <deque>
#include <math.h>

using namespace std;

const bool OUTPUT_GAME_DATA = 1;
const bool USE_HARDCODED_INPUT = 0;

const int INVALID_ID = -1;
const int INVALID_NODE_DEPTH = -1;
const int TREE_ROOT_NODE_DEPTH = 1;
const int ZERO_CHAR = '0';
const int INVALID_COORD = -1;
const int DIRECTIONS_COUNT = 8;
const int RIGHT_ANGLE = 90;

const float MARS_GRAVITY = 3.711f;

const float PI = 3.14159265f;

enum ComponentType {
	CT_HORIZONTAL = 0,
	CT_VERTICAL,
};

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

class Coords {
public:
	Coords();
	Coords(int xCoord, int yCoord);

	int getXCoord() const {
		return xCoord;
	}

	int getYCoord() const {
		return yCoord;
	}

	void setXCoord(int xCoord) { this->xCoord = xCoord; }
	void setYCoord(int yCoord) { this->yCoord = yCoord; }

	Coords& operator=(const Coords& other);
	bool operator==(const Coords& other);
	Coords operator+(const Coords& other);
	Coords& operator+=(const Coords& other);

	bool isValid() const;
	void debug() const;
private:
	int xCoord;
	int yCoord;
};

//*************************************************************************************************************
//*************************************************************************************************************

Coords::Coords() :
	xCoord(INVALID_COORD),
	yCoord(INVALID_COORD)
{
}

//*************************************************************************************************************
//*************************************************************************************************************

Coords::Coords(
	int xCoord,
	int yCoord
) :
	xCoord(xCoord),
	yCoord(yCoord)
{
}

//*************************************************************************************************************
//*************************************************************************************************************

Coords& Coords::operator=(const Coords& other) {
	if (this != &other) {
		xCoord = other.xCoord;
		yCoord = other.yCoord;
	}

	return *this;
}

//*************************************************************************************************************
//*************************************************************************************************************

bool Coords::operator==(const Coords& other) {
	return (xCoord == other.xCoord) && (yCoord == other.yCoord);
}

//*************************************************************************************************************
//*************************************************************************************************************

Coords Coords::operator+(const Coords& other) {
	return Coords(xCoord + other.xCoord, yCoord + other.yCoord);
}

//*************************************************************************************************************
//*************************************************************************************************************

Coords& Coords::operator+=(const Coords& other) {
	xCoord += other.xCoord;
	yCoord += other.yCoord;

	return *this;
}

//*************************************************************************************************************
//*************************************************************************************************************

bool Coords::isValid() const {
	return INVALID_COORD != xCoord && INVALID_COORD != yCoord;
}

//*************************************************************************************************************
//*************************************************************************************************************

void Coords::debug() const {
	cerr << "Position: X=" << xCoord << ", Y=" << yCoord << endl;
}

//*************************************************************************************************************
//*************************************************************************************************************

enum Direction {
	DIR_INVALID = -1,
	DIR_N = 0,
	DIR_NE,
	DIR_E,
	DIR_SE,
	DIR_S,
	DIR_SW,
	DIR_W,
	DIR_NW,
};

//*************************************************************************************************************
//*************************************************************************************************************

Coords DIRECTIONS[DIRECTIONS_COUNT] = {
	Coords(0, -1), // N
	Coords(1, -1), // NE
	Coords(1,  0), // E
	Coords(1,  1), // SE
	Coords(0,  1), // S
	Coords(-1,  1), // SW
	Coords(-1,  0), // W
	Coords(-1, -1)  // NW
};

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

class Shuttle {
public:
	Shuttle();

	Coords getPosition() {
		return position;
	}

	Coords getPreviousPosition() {
		return previousPosition;
	}


	int getHSpeed() {
		return hSpeed;
	}

	int getVSpeed() {
		return vSpeed;
	}

	int getFuel() {
		return fuel;
	}

	int getRotate() {
		return rotate;
	}

	int getPower() {
		return power;
	}

	void setPosition(Coords position) { this->position = position; }
	void setPreviousPosition(Coords previousPosition) { this->previousPosition = previousPosition; }
	void setHSpeed(int hSpeed) { this->hSpeed = hSpeed; }
	void setVSpeed(int vSpeed) { this->vSpeed = vSpeed; }
	void setFuel(int fuel) { this->fuel = fuel; }
	void setRotate(int rotate) { this->rotate = rotate; }
	void setPower(int power) { this->power = power; }

	void calculateComponents(
		int newAngle,
		int newPower,
		int initialSpeed,
		ComponentType componentType,
		float& displacement,
		float& acceleration
	);

	void simulate(int rotateAngle, int thrustPower);

	void print() const;

private:
	Coords position;
	Coords previousPosition;
	int hSpeed; // the horizontal speed (in m/s), can be negative.
	int vSpeed; // the vertical speed (in m/s), can be negative.
	int fuel; // the quantity of remaining fuel in liters.
	int rotate; // the rotation angle in degrees (-90 to 90).
	int power; // the thrust power (0 to 4).
};

//*************************************************************************************************************
//*************************************************************************************************************

Shuttle::Shuttle() :
	position(),
	hSpeed(0),
	vSpeed(0),
	fuel(0),
	rotate(0),
	power(0)
{

}

//*************************************************************************************************************
//*************************************************************************************************************

void Shuttle::calculateComponents(
	int newAngle,
	int newPower,
	int initialSpeed,
	ComponentType componentType,
	float& displacement,
	float& acceleration
) {
	int theta = RIGHT_ANGLE + newAngle;
	float rad = theta * PI / (2 * RIGHT_ANGLE);

	float mult = cos(rad);
	if (CT_VERTICAL == componentType) {
		mult = sin(rad);
	}

	acceleration = mult * (float)newPower;

	if (CT_VERTICAL == componentType) {
		acceleration -= MARS_GRAVITY;
	}

	displacement = (float)initialSpeed + (0.5 * acceleration);
}

//*************************************************************************************************************
//*************************************************************************************************************

void Shuttle::simulate(int rotateAngle, int thrustPower) {
	int newAngle = /*rotate +*/ rotateAngle;
	int newPower = /*power +*/ thrustPower;
	int newFuel = fuel - newPower; 

	float displacementX = 0.f;
	float displacementY = 0.f;

	float accelerationX = 0.f;
	float accelerationY = 0.f;

	calculateComponents(newAngle, newPower, hSpeed, CT_HORIZONTAL, displacementX, accelerationX);
	calculateComponents(newAngle, newPower, vSpeed, CT_VERTICAL, displacementY, accelerationY);

	float newX = (float)position.getXCoord() + displacementX;
	float newY = (float)position.getYCoord() + displacementY;

	float newHSpeed = (float)hSpeed + accelerationX;
	float newVSpeed = (float)vSpeed + accelerationY;

	cerr << "X=" << newX << "m, Y=" << newY << "m, ";
	cerr << "HSPeed=" << newHSpeed << "m/s VSpeed=" << newVSpeed << "m/s\n";
	cerr << "Fule=" << newFuel << "l, Angle=" << newAngle << ", Power=" << newPower << "m/s2\n";
}

//*************************************************************************************************************
//*************************************************************************************************************

void Shuttle::print() const {
}

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

class Game {
public:
	Game();
	~Game();

	void initGame();
	void gameBegin();
	void gameLoop();
	void getGameInput();
	void getTurnInput();
	void turnBegin();
	void makeTurn();
	void turnEnd();
	void play();

	void debug() const;

private:
	int turnsCount;

	Shuttle* shuttle;
};

//*************************************************************************************************************
//*************************************************************************************************************

Game::Game() : turnsCount(0) {
}

//*************************************************************************************************************
//*************************************************************************************************************

Game::~Game() {
	if (shuttle) {
		delete shuttle;
		shuttle = NULL;
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::initGame() {
	shuttle = new Shuttle();
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::gameBegin() {
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::gameLoop() {
	while (true) {
		getTurnInput();
		turnBegin();
		makeTurn();
		turnEnd();
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::getGameInput() {
	if (!USE_HARDCODED_INPUT) {
		int surfaceN; // the number of points used to draw the surface of Mars.
		cin >> surfaceN; cin.ignore();

		for (int i = 0; i < surfaceN; i++) {
			int landX; // X coordinate of a surface point. (0 to 6999)
			int landY; // Y coordinate of a surface point. By linking all the points together in a sequential fashion, you form the surface of Mars.
			cin >> landX >> landY; cin.ignore();
		}
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::getTurnInput() {
	int X;
	int Y;
	int hSpeed; // the horizontal speed (in m/s), can be negative.
	int vSpeed; // the vertical speed (in m/s), can be negative.
	int fuel; // the quantity of remaining fuel in liters.
	int rotate; // the rotation angle in degrees (-90 to 90).
	int power; // the thrust power (0 to 4).

	if (USE_HARDCODED_INPUT) {
		// X = 2500m, Y = 2699m, HSpeed = 0m / s VSpeed = -3m / s
		// Fuel = 549l, Angle = -15°, Power = 1 (1.0m / s2)

		X = 2500;
		Y = 2700;
		hSpeed = 0;
		vSpeed = 0;
		fuel = 550;
		rotate = 0;
		power = 0;

		shuttle->setPreviousPosition(Coords(X, Y));
	}
	else {
		cin >> X >> Y >> hSpeed >> vSpeed >> fuel >> rotate >> power; cin.ignore();
	}

	shuttle->setPosition(Coords(X, Y));
	shuttle->setHSpeed(hSpeed);
	shuttle->setVSpeed(vSpeed);
	shuttle->setFuel(fuel);
	shuttle->setRotate(rotate);
	shuttle->setPower(power);
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::turnBegin() {
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::makeTurn() {
	shuttle->simulate(-15, 1);

	cout << "-15 1" << endl;
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::turnEnd() {
	++turnsCount;
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::play() {
	initGame();
	getGameInput();
	gameBegin();
	gameLoop();
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::debug() const {
}

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

#ifdef TESTS
#include "debug.h"
#endif // TESTS

int main(int argc, char** argv) {
#ifdef TESTS
	doctest::Context context;
	int res = context.run();
#else
	Game game;
	game.play();
#endif // TESTS

	return 0;
}