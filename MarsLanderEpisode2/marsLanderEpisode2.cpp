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
const bool USE_HARDCODED_INPUT = 1;

const int INVALID_ID = -1;
const int INVALID_NODE_DEPTH = -1;
const int TREE_ROOT_NODE_DEPTH = 1;
const int ZERO_CHAR = '0';
const int INVALID_COORD = -1;
const int DIRECTIONS_COUNT = 8;

const float MARS_GRAVITY = 3.711f;

const float PI = 3.14159265f;

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

	void simulate(int rotateAngle, int thrustPower);

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

//def getNextState(lander : Lander, action : Action) : Lander = {
//	var nextP = lander.power + action.power
//	var nextFuel = lander.fuel - nextP
//	var nextAngle = lander.angle + action.angle
//	var nextX = 2 * lander.x - lander.prevX - nextP * sin(toRadians(nextAngle))
//	var nextY = 2 * lander.y - lander.prevY - g + nextP * cos(toRadians(nextAngle))
//	var nextVx = lander.hSpeed - nextP * sin(toRadians(nextAngle))
//	var nextVy = lander.vSpeed - g + nextP * cos(toRadians(nextAngle))
//	var nextLander = Lander(nextX, nextY, lander.x, lander.y, nextVx, nextVy, nextFuel, nextAngle, nextP)
//	nextLander
//}

void Shuttle::simulate(int rotateAngle, int thrustPower) {
	// X = 2501m, Y = 2695m, HSpeed = 1m / s VSpeed = -5m / s
	// Fuel = 548l, Angle = -15°, Power = 1 (1.0m / s2)

	const float thetha = float(90 + rotateAngle) * PI / 180.f;

	const float g = MARS_GRAVITY;
	const int t = 1;
	const float initalVelocity = sqrtf(float((hSpeed * hSpeed) + (vSpeed * vSpeed)));
	const float v = initalVelocity /*+ thrustPower*/;

	const float vx = v * cos(thetha);
	const float vy = v * sin(thetha);

	const float x = vx * t;
	const float y = (vy * t) - (g / 2.f);

	const int xOffset = int(x);
	const int yOffset = int(y);

	const float horizontalVelocity = x;
	const float verticalVelocity = vy - (g * t);

	const int newHSpeed = int(horizontalVelocity);
	const int newVSpeed = int(verticalVelocity);

	int nextPower = power + thrustPower;
	int nextFuel = fuel - nextPower;
	int nextAngle = rotate + rotateAngle;
	int nextX = 2 * position.getXCoord() - previousPosition.getXCoord() - nextPower * sin(nextAngle * PI / 180.f);
	int nextY = 2 * position.getYCoord() - previousPosition.getYCoord() - g + nextPower * cos(nextAngle * PI / 180.f);
	int nextVx = hSpeed - nextPower * sin(nextAngle * PI / 180.f);
	int nextVy = vSpeed - g + nextPower * cos(nextAngle * PI / 180.f);

	int debug = 0;
	++debug;
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
		Y = 2699;
		hSpeed = 0;
		vSpeed = -3;
		fuel = 549;
		rotate = -15;
		power = 1;

		shuttle->setPreviousPosition(Coords(2500, 2700));
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
	shuttle->simulate(0, 1);

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