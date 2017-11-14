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

typedef float Coord;
const Coord INVALID_COORD = -1.f;

class Coords {
public:
	Coords();
	Coords(Coord xCoord, Coord yCoord);

	Coord getXCoord() const {
		return xCoord;
	}

	Coord getYCoord() const {
		return yCoord;
	}

	void setXCoord(Coord xCoord) { this->xCoord = xCoord; }
	void setYCoord(Coord yCoord) { this->yCoord = yCoord; }

	Coords& operator=(const Coords& other);
	bool operator==(const Coords& other);
	Coords operator+(const Coords& other);
	Coords& operator+=(const Coords& other);

	bool isValid() const;
	void debug() const;
private:
	Coord xCoord;
	Coord yCoord;
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
	Coord xCoord,
	Coord yCoord
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

class Line {
public:
	Line();
	Line(const Coords& point0, const Coords& point1);
	~Line();

	bool pointBelow(const Coords& landerPoint);

private:
	Coords point0;
	Coords point1;
	bool landingZone;
};

//*************************************************************************************************************
//*************************************************************************************************************

Line::Line() {

}

//*************************************************************************************************************
//*************************************************************************************************************

Line::Line(const Coords& point0, const Coords& point1) :
	point0(point0),
	point1(point1)
{

}

//*************************************************************************************************************
//*************************************************************************************************************

Line::~Line() {

}

//*************************************************************************************************************
//*************************************************************************************************************

bool Line::pointBelow(const Coords& landerPoint) {
	bool below = false;

	Coord landerX = landerPoint.getXCoord();
	Coord lineX0 = point0.getXCoord();
	Coord lineX1 = point1.getXCoord();

	if (landerX >= lineX0 && landerX < lineX1) {
		Coord landerY = landerPoint.getYCoord();
		Coord lineY0 = point0.getYCoord();
		Coord lineY1 = point1.getYCoord();

		float m = (lineY1 - lineY0) / (lineX1 - lineX0);
		float b = lineY1 - (m * lineX1);

		if (landerY <= (m * landerX) + b) {
			below = true;
		}
	}

	return below;
}

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

typedef vector<Line> Lines;

class Surface {
public:
	Surface();
	~Surface();

	bool collisionWithSurface(const Coords& landerPoint);
	void addLine(const Coords& point0, const Coords& point1);
private:
	Lines lines;
	Line landingZone;
};

//*************************************************************************************************************
//*************************************************************************************************************

Surface::Surface() :
	lines()
{

}

//*************************************************************************************************************
//*************************************************************************************************************

Surface::~Surface() {

}

//*************************************************************************************************************
//*************************************************************************************************************

bool Surface::collisionWithSurface(const Coords& landerPoint) {
	int linesBelowLander = 0;
	
	for (size_t lineIdx = 0; lineIdx < lines.size(); ++lineIdx) {
		if (lines[lineIdx].pointBelow(landerPoint)) {
			++linesBelowLander;
		}
	}

	return !(linesBelowLander % 2);
}

//*************************************************************************************************************
//*************************************************************************************************************

void Surface::addLine(const Coords& point0, const Coords& point1) {
	Line line(point0, point1);
	lines.push_back(line);

	if (point0.getYCoord() == point1.getYCoord()) {
		landingZone = line;
	}
}

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


	float getHSpeed() {
		return hSpeed;
	}

	float getVSpeed() {
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
	void setHSpeed(float hSpeed) { this->hSpeed = hSpeed; }
	void setVSpeed(float vSpeed) { this->vSpeed = vSpeed; }
	void setFuel(int fuel) { this->fuel = fuel; }
	void setRotate(int rotate) { this->rotate = rotate; }
	void setPower(int power) { this->power = power; }

	void calculateComponents(
		int newAngle,
		int newPower,
		float initialSpeed,
		ComponentType componentType,
		float& displacement,
		float& acceleration
	);

	void simulate(int rotateAngle, int thrustPower);

	void print() const;

private:
	Coords position;
	Coords previousPosition;
	float hSpeed; // the horizontal speed (in m/s), can be negative.
	float vSpeed; // the vertical speed (in m/s), can be negative.
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
	float initialSpeed,
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

	displacement = (float)initialSpeed + (.5f * acceleration);
}

//*************************************************************************************************************
//*************************************************************************************************************

void Shuttle::simulate(int rotateAngle, int thrustPower) {
	int simTurns = 0;

	while (simTurns++ <= 35) {
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

		position.setXCoord(newX);
		position.setYCoord(newY);

		hSpeed = newHSpeed;
		vSpeed = newVSpeed;

		cout << "Turn: " << simTurns << endl;
		cout << "X=" << newX << "m, Y=" << newY << "m, ";
		cout << "HSPeed=" << newHSpeed << "m/s VSpeed=" << newVSpeed << "m/s\n";
		cout << "Fuel=" << newFuel << "l, Angle=" << newAngle << ", Power=" << newPower << "m/s2\n";
		cout << endl << endl;
	}
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
	Surface* surface;
};

//*************************************************************************************************************
//*************************************************************************************************************

Game::Game() :
	turnsCount(0),
	shuttle(NULL),
	surface(NULL)
{
}

//*************************************************************************************************************
//*************************************************************************************************************

Game::~Game() {
	if (shuttle) {
		delete shuttle;
		shuttle = NULL;
	}

	if (surface) {
		delete surface;
		surface = NULL;
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::initGame() {
	shuttle = new Shuttle();
	surface = new Surface();
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

		Coords point0;

		for (int i = 0; i < surfaceN; i++) {
			int landX; // X coordinate of a surface point. (0 to 6999)
			int landY; // Y coordinate of a surface point. By linking all the points together in a sequential fashion, you form the surface of Mars.
			cin >> landX >> landY; cin.ignore();

			Coords point1((float)landX, (float)landY);

			if (0 != i) {
				surface->addLine(point0, point1);
			}

			point0 = point1;
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
		X = 2500;
		Y = 2700;
		hSpeed = 0;
		vSpeed = 0;
		fuel = 550;
		rotate = 0;
		power = 0;
	}
	else {
		cin >> X >> Y >> hSpeed >> vSpeed >> fuel >> rotate >> power; cin.ignore();
	}

	shuttle->setPosition(Coords((float)X, (float)Y));
	shuttle->setHSpeed((float)hSpeed);
	shuttle->setVSpeed((float)vSpeed);
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
	//shuttle->simulate(-15, 1);

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