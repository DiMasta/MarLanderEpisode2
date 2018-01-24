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
#include <fstream>
#include <random>
#include <chrono>
#include <iterator>

#define SVG
#define REDIRECT_CIN_FROM_FILE
#define REDIRECT_COUT_TO_FILE
#define SIMULATION_OUTPUT

#ifdef SVG
#include "SVGManager.h"
#endif // SVG

using namespace std;

const bool OUTPUT_GAME_DATA = 0;
const bool USE_OLDSCHOOL_RAND = 1;

const int MAP_WIDTH = 7000;
const int MAP_HEIGHT = 3000;
const int MAX_DISTANCE = MAP_WIDTH * MAP_WIDTH;
const int ASPECT = 10;
const int INVALID_ID = -1;
const int INVALID_NODE_DEPTH = -1;
const int TREE_ROOT_NODE_DEPTH = 1;
const int ZERO_CHAR = '0';
const int DIRECTIONS_COUNT = 8;
const int RIGHT_ANGLE = 90;

const float PI = 3.14159265f;
const float BEST_CHROMOSOMES_PERCENT = .3f;
const float OTHERS_CHROMOSOMES_PERCENT = .2f;
const float MARS_GRAVITY = 3.711f;
const float SAVE_DISTANCE_TO_LANDING_ZONE = 50.f;
const float MAX_V_ABS_SPEED = 40.f;
const float MAX_H_ABS_SPEED = 20.f;
const float MAX_OUT_OF_LANDING_AREA_SCORE = 100.f;
const float MAX_LANDING_AREA_BIG_SPEED_SCORE = 200.f;
const float MAX_LANDING_AREA_NORMAL_SPEED_SCORE = 300.f;
const float SPEED_PENALTY_WEIGHT = .1f;
const float CROSSOVER_GENE_PROB = .5f;
const float FLOAT_MAX_RAND = static_cast<float>(RAND_MAX);

const string INPUT_FILE_NAME = "input.txt";
const string OUTPUT_FILE_NAME = "output.txt";

const int CHROMOSOME_SIZE = 100;//300
const int POPULATION_SIZE = 100;
const int MAX_POPULATION = 100;
const int CHILDREN_COUNT = POPULATION_SIZE;

const int INVALID_ROTATION_ANGLE = 100;
const int INVALID_POWER = -1;
const int MIN_ROTATION_ANGLE = -90;
const int MAX_ROTATION_ANGLE = 90;
const int MIN_ROTATION_ANGLE_STEP = -15;
const int MAX_ROTATION_ANGLE_STEP = 15;
const int MIN_POWER = 0;
const int MAX_POWER = 4;
const int MIN_POWER_STEP = -1;
const int MAX_POWER_STEP = 1;
const int MAX_V_SPEED_FOR_LANDING = 40;
const int MAX_H_SPEED_FOR_LANDING = 20;

enum ComponentType {
	CT_INVALID = -1,
	CT_HORIZONTAL = 0,
	CT_VERTICAL,
};

enum LandingZoneDirection {
	LZD_INVALID = -1,
	LZD_LEFT = 0,
	LZD_HERE,
	LZD_RIGHT,
};

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

namespace Math {
	float randomFloatBetween0and1() {
		return static_cast<float>(rand()) / FLOAT_MAX_RAND;
	}
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

	friend Coord distance(const Coords& point0, const Coords& point1);
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

Coord distance(const Coords& point0, const Coords& point1) {
	Coord lineXLenght = static_cast<Coord>(abs(point0.xCoord - point1.xCoord));
	Coord lineYLenght = static_cast<Coord>(abs(point0.yCoord - point1.yCoord));

	Coord distance = static_cast<Coord>(sqrt((lineXLenght * lineXLenght) + (lineYLenght * lineYLenght)));

	return distance;
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
	Line(const Coords& point0, const Coords& point1, int landingZoneDirection);
	~Line();

	Coords getPoint0() const {
		return point0;
	}

	Coords getPoint1() const {
		return point1;
	}

	int getLandingZoneDirection() const {
		return landingZoneDirection;
	}

	int getLenght() const {
		return lenght;
	}

	void setPoint0(const Coords& point0) { this->point0 = point0; }
	void setPoint1(const Coords& point1) { this->point1 = point1; }
	void setLandingZoneDirection(int landingZoneDirection) { this->landingZoneDirection = landingZoneDirection; }
	void setLenght(int lenght) { this->lenght = lenght; }

	Coord distanceToPoint(const Coords& point) const;
	bool pointBelow(const Coords& landerPoint) const;

private:
	Coords point0;
	Coords point1;
	int landingZoneDirection;
	int lenght;
	Coord m;
	Coord b;
};

//*************************************************************************************************************
//*************************************************************************************************************

Line::Line() {

}

//*************************************************************************************************************
//*************************************************************************************************************

Line::Line(const Coords& point0, const Coords& point1, int landingZoneDirection) :
	point0(point0),
	point1(point1),
	landingZoneDirection(landingZoneDirection)
{
	Coord x0 = point0.getXCoord();
	Coord y0 = point0.getYCoord();
	Coord x1 = point1.getXCoord();
	Coord y1 = point1.getYCoord();

	int lineX = static_cast<int>(abs(x0 - x1));
	int lineY = static_cast<int>(abs(y0 - y1));

	lenght = static_cast<int>(sqrt((lineX * lineX) + (lineY * lineY)));

	m = (y1 - y0) / (x1 - x0);
	b = y0 - (m * x0);
}

//*************************************************************************************************************
//*************************************************************************************************************

Line::~Line() {

}

//*************************************************************************************************************
//*************************************************************************************************************

Coord Line::distanceToPoint(const Coords& point) const {
	Coord tempLineM = -(1 / m);
	Coord tempLineB = point.getYCoord() - (tempLineM * point.getXCoord());

	Coord intersectXCoord = (tempLineB - b) / (m - tempLineM);
	Coord intersectYCoord = (tempLineM * intersectXCoord) + tempLineB;

	Coords intersectionPoint(intersectXCoord, intersectYCoord);

	return distance(point, intersectionPoint);
}

//*************************************************************************************************************
//*************************************************************************************************************

bool Line::pointBelow(const Coords& landerPoint) const {
	bool below = false;

	Coord landerX = landerPoint.getXCoord();
	Coord line0X = point0.getXCoord();
	Coord line1X = point1.getXCoord();

	if (landerX >= line0X && landerX < line1X) {
		Coord landerY = landerPoint.getYCoord();
		Coord line0Y = point0.getYCoord();
		Coord line1Y = point1.getYCoord();

		//	Line[{x1, y1}, { x2,y2 }]
		//	Points{ xA,yA }, { xB,yB } ...
		//
		//	v1 = { x2 - x1, y2 - y1 }   # Vector 1
		//	v2 = { x2 - xA, y2 - yA }   # Vector 1
		//	xp = v1.x*v2.y - v1.y*v2.x  # Cross product
		//	if xp > 0:
		//		print 'on one side'
		//	elif xp < 0 :
		//		print 'on the other'
		//	else:
		//		print 'on the same line!'

		Coords v1(line1X - line0X, line1Y - line0Y);
		Coords v2(line1X - landerX, line1Y - landerY);

		int xp = static_cast<int>(v1.getXCoord() * v2.getYCoord() - v1.getYCoord() * v2.getXCoord());

		if (xp < 0) {
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

	Lines getLines() const {
		return lines;
	}

	Line getLandingZone() const {
		return landingZone;
	}

	float getMaxDistance() const {
		return maxDistance;
	}

	void setLines(const Lines& lines) { this->lines = lines; }
	void setLandingZone(const Line& landingZone) { this->landingZone = landingZone; }
	void setMaxDistance(float maxDistance) { this->maxDistance = maxDistance; }

	int getLinesCount() const;
	Line getLine(int lineIdx) const;

	void collisionWithSurface(
		const Coords& point0,
		const Coords& point1,
		Coords& collisionPoint,
		int& crashLineIdx
	);

	void addLine(
		const Coords& point0,
		const Coords& point1,
		int landingZoneDirection,
		bool& landingZoneFound
	);

	float findDistanceToLandingZone(const Coords& from, int crashLineIdx) const;

	string constructSVGData(const SVGManager& svgManager) const;

private:
	Lines lines;
	Line landingZone;
	float maxDistance;
};

//*************************************************************************************************************
//*************************************************************************************************************

Surface::Surface() :
	lines(),
	landingZone(),
	maxDistance(0.f)
{

}

//*************************************************************************************************************
//*************************************************************************************************************

Surface::~Surface() {

}

//*************************************************************************************************************
//*************************************************************************************************************

int Surface::getLinesCount() const {
	return int(lines.size());
}

//*************************************************************************************************************
//*************************************************************************************************************

Line Surface::getLine(int lineIdx) const {
	return lines[lineIdx];
}

//*************************************************************************************************************
//*************************************************************************************************************

void Surface::collisionWithSurface(
	const Coords& point0,
	const Coords& point1,
	Coords& collisionPoint,
	int& crashLineIdx
) {
	if (point1.getXCoord() >= 0 && point1.getXCoord() <= MAP_WIDTH) {
		for (size_t lineIdx = 0; lineIdx < lines.size(); ++lineIdx) {
			const Line& line = lines[lineIdx];

			const Coord p0x = point0.getXCoord();
			const Coord p0y = point0.getYCoord();
			const Coord p1x = point1.getXCoord();
			const Coord p1y = point1.getYCoord();

			const Coord p2x = line.getPoint0().getXCoord();
			const Coord p2y = line.getPoint0().getYCoord();
			const Coord p3x = line.getPoint1().getXCoord();
			const Coord p3y = line.getPoint1().getYCoord();

			const Coord s1x = p1x - p0x;
			const Coord s1y = p1y - p0y;

			const Coord s2x = p3x - p2x;
			const Coord s2y = p3y - p2y;

			const Coord s = (-s1y * (p0x - p2x) + s1x * (p0y - p2y)) / (-s2x * s1y + s1x * s2y);
			const Coord t = (s2x * (p0y - p2y) - s2y * (p0x - p2x)) / (-s2x * s1y + s1x * s2y);

			if (s >= 0.f && s <= 1.f && t >= 0.f && t <= 1.f) {
				collisionPoint.setXCoord(p0x + (t * s1x));
				collisionPoint.setYCoord(p0y + (t * s1y));

				crashLineIdx = lineIdx;
			}
		}
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void Surface::addLine(
	const Coords& point0,
	const Coords& point1,
	int landingZoneDirection,
	bool& landingZoneFound
) {
	Line line(point0, point1, landingZoneDirection);

	if (point0.getYCoord() == point1.getYCoord()) {
		landingZone = line;
		landingZoneFound = true;
		line.setLandingZoneDirection(LZD_HERE);
	}

	lines.push_back(line);
}

//*************************************************************************************************************
//*************************************************************************************************************

string Surface::constructSVGData(const SVGManager& svgManager) const {
#ifdef SVG
	string svgStr = POLYLINE_BEGIN;

	for (size_t lineIdx = 0; lineIdx < lines.size(); ++lineIdx) {
		Line line = lines[lineIdx];
		Coord startX = line.getPoint0().getXCoord();
		Coord startY = MAP_HEIGHT - line.getPoint0().getYCoord();
		Coord endX = line.getPoint1().getXCoord();
		Coord endY = MAP_HEIGHT - line.getPoint1().getYCoord();

		svgStr.append(to_string(startX));
		svgStr.append(",");
		svgStr.append(to_string(startY));
		svgStr.append(" ");
		svgStr.append(to_string(endX));
		svgStr.append(",");
		svgStr.append(to_string(endY));
		svgStr.append(" ");
	}
	svgStr.append("\" ");

	svgStr.append(STYLE_BEGIN);
	svgStr.append(FILL_NONE);
	string strokeRGB = svgManager.constructStrokeForRGB(255, 0, 0);
	svgStr.append(strokeRGB);
	svgStr.append(";");

	string strokeWidth = svgManager.constructStrokeWidth(10);
	svgStr.append(strokeWidth);
	svgStr.append(ATTRIBUTE_END);

	svgStr.append(POLYLINE_END);

	return svgStr;
#endif // SVG

	return "";
}

//*************************************************************************************************************
//*************************************************************************************************************

float Surface::findDistanceToLandingZone(const Coords& from, int crashLineIdx) const {
	float distTolandingZone = 0.f;

	if (INVALID_ID != crashLineIdx) {
		const Line& crashedLine = lines[crashLineIdx];

		if (crashedLine.getLandingZoneDirection() != LZD_HERE) {
			Coords pointToLandingZone = crashedLine.getPoint0();
			if (LZD_RIGHT == crashedLine.getLandingZoneDirection()) {
				pointToLandingZone = crashedLine.getPoint1();
			}
			else if (LZD_HERE == crashedLine.getLandingZoneDirection()) {
				pointToLandingZone = from;
			}

			distTolandingZone = distance(from, pointToLandingZone);
			const Line* line = &crashedLine;

			int lineIdx = crashLineIdx;
			while (true) {
				const int lzd = line->getLandingZoneDirection();

				if (LZD_RIGHT == lzd) {
					++lineIdx;
				}
				else if (LZD_LEFT == lzd) {
					--lineIdx;
				}

				line = &lines[lineIdx];

				if (LZD_HERE == line->getLandingZoneDirection()) {
					break;
				}

				distTolandingZone += line->getLenght();
			}
		}
	}
	else {
		distTolandingZone = MAX_DISTANCE;
	}

	return distTolandingZone;
}

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

class Shuttle {
public:
	Shuttle();
	Shuttle(const Shuttle& shuttle);

	Coords getPosition() const {
		return position;
	}

	float getHSpeed() const {
		return hSpeed;
	}

	float getVSpeed() const {
		return vSpeed;
	}

	int getFuel() const {
		return fuel;
	}

	int getRotate() const {
		return rotate;
	}

	int getPower() const {
		return power;
	}

	int getInitialFuel() const {
		return initialFuel;
	}

	void setPosition(Coords position) { this->position = position; }
	void setHSpeed(float hSpeed) { this->hSpeed = hSpeed; }
	void setVSpeed(float vSpeed) { this->vSpeed = vSpeed; }
	void setFuel(int fuel) { this->fuel = fuel; }
	void setRotate(int rotate) { this->rotate = rotate; }
	void setPower(int power) { this->power = power; }
	void setInitialFuel(int initialFuel) { this->initialFuel = initialFuel; }

	void calculateComponents(
		int newAngle,
		int newPower,
		float initialSpeed,
		ComponentType componentType,
		float& displacement,
		float& acceleration
	);

	int clampRotateAngle(int newRotateAngle) const;
	int clampPower(int newPower) const;
	void simulate(int rotateAngle, int thrustPower);
	void print() const;
	bool goodForLanding(const Line& landingZone) const;
	void getTitleLines(vector<string>& titleLines) const;

private:
	Coords position;
	float hSpeed; // the horizontal speed (in m/s), can be negative.
	float vSpeed; // the vertical speed (in m/s), can be negative.
	int fuel; // the quantity of remaining fuel in liters.
	int rotate; // the rotation angle in degrees (-90 to 90).
	int power; // the thrust power (0 to 4).
	int initialFuel;
};

//*************************************************************************************************************
//*************************************************************************************************************

Shuttle::Shuttle() :
	position(),
	hSpeed(0),
	vSpeed(0),
	fuel(0),
	rotate(0),
	power(0),
	initialFuel(0)
{

}

//*************************************************************************************************************
//*************************************************************************************************************

Shuttle::Shuttle(const Shuttle& shuttle) {
	position = shuttle.position;
	hSpeed = shuttle.hSpeed;
	vSpeed = shuttle.vSpeed;
	fuel = shuttle.fuel;
	rotate = shuttle.rotate;
	power = shuttle.power;
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

int Shuttle::clampRotateAngle(int newRotateAngle) const {
	int clampedAngle = newRotateAngle;

	int angleDiff = newRotateAngle - rotate;
	if (angleDiff > MAX_ROTATION_ANGLE_STEP) {
		clampedAngle = rotate + MAX_ROTATION_ANGLE_STEP;
	}
	else if (angleDiff < MIN_ROTATION_ANGLE_STEP) {
		clampedAngle = rotate + MIN_ROTATION_ANGLE_STEP;
	}

	return clampedAngle;
}

//*************************************************************************************************************
//*************************************************************************************************************

int Shuttle::clampPower(int newPower) const {
	int clampedPower = newPower;

	int powerDiff = newPower - power;
	if (powerDiff > MAX_POWER_STEP) {
		clampedPower = power + MAX_POWER_STEP;
	}
	else if (powerDiff < MIN_POWER_STEP) {
		clampedPower = power + MIN_POWER_STEP;
	}

	return clampedPower;
}

//*************************************************************************************************************
//*************************************************************************************************************

#ifdef SIMULATION_OUTPUT
int turn = 1;
#endif // SIMULATION_OUTPUT

void Shuttle::simulate(int rotateAngle, int thrustPower) {
	int newAngle = clampRotateAngle(rotateAngle);
	int newPower = clampPower(thrustPower);

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

	fuel = newFuel;
	power = newPower;
	rotate = newAngle;

#ifdef SIMULATION_OUTPUT
	//cout << "Turn=" << turn++ << endl;
	//cout << "X=" << newX << "m, Y=" << newY << "m, ";
	//cout << "HSPeed=" << newHSpeed << "m/s VSpeed=" << newVSpeed << "m/s\n";
	//cout << "Fuel=" << newFuel << "l, Angle=" << newAngle << ", Power=" << newPower << "m/s2\n";
	//cout << endl << endl;
#endif // SIMULATION_OUTPUT
}

//*************************************************************************************************************
//*************************************************************************************************************

void Shuttle::print() const {
}

//*************************************************************************************************************
//*************************************************************************************************************

bool Shuttle::goodForLanding(const Line& landingZone) const {
	bool res = false;

	Coord shuttleX = position.getXCoord();
	Coord shuttleY = position.getYCoord();

	Coord landingZoneX0 = landingZone.getPoint0().getXCoord();
	Coord landingZoneX1 = landingZone.getPoint1().getXCoord();
	Coord landingZoneY = landingZone.getPoint0().getYCoord();

	if (shuttleX > landingZoneX0 && shuttleX < landingZoneX1) {
		Coord distToLandingZone = shuttleY - landingZoneY;
		if (distToLandingZone < SAVE_DISTANCE_TO_LANDING_ZONE) {
			if (rotate >= MIN_ROTATION_ANGLE_STEP && rotate <= MAX_ROTATION_ANGLE_STEP) {
				if (hSpeed <= MAX_H_SPEED_FOR_LANDING && vSpeed <= MAX_V_SPEED_FOR_LANDING) {
					res = true;
				}
			}
		}
	}

	return res;
}

//*************************************************************************************************************
//*************************************************************************************************************

void Shuttle::getTitleLines(vector<string>& titleLines) const {
	titleLines.clear();

	string positionLine = "Shuttle position: (";
	positionLine.append(to_string(static_cast<int>(position.getXCoord())));
	positionLine.append(", ");
	positionLine.append(to_string(static_cast<int>(position.getYCoord())));
	positionLine.append(")");
	titleLines.push_back(positionLine);

	string rotationLine = "Rotation: ";
	rotationLine.append(to_string(rotate));
	titleLines.push_back(rotationLine);

	string powerLine = "Power: ";
	powerLine.append(to_string(power));
	titleLines.push_back(powerLine);

	string hSpeedLine = "HSpeed: ";
	hSpeedLine.append(to_string(static_cast<int>(hSpeed)));
	titleLines.push_back(hSpeedLine);

	string vSpeedLine = "VSpeed: ";
	vSpeedLine.append(to_string(static_cast<int>(vSpeed)));
	titleLines.push_back(vSpeedLine);
}

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

struct Gene {
	Gene();
	Gene(int rotate, int power);

	Gene& operator=(const Gene& other);

	int rotate;
	int power;
};

//*************************************************************************************************************
//*************************************************************************************************************

Gene::Gene() :
	rotate(INVALID_ROTATION_ANGLE),
	power(INVALID_POWER)
{
}

//*************************************************************************************************************
//*************************************************************************************************************

Gene::Gene(
	int rotate,
	int power
) :
	rotate(rotate),
	power(power)
{
}

//*************************************************************************************************************
//*************************************************************************************************************

Gene& Gene::operator=(const Gene& other) {
	if (this != &other) {
		rotate = other.rotate;
		power = other.power;
	}

	return *this;
}

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

typedef vector<Gene> Genes;
typedef vector<Coords> Path;

class Chromosome {
public:
	Chromosome();
	~Chromosome();

	Shuttle getShuttle() const {
		return shuttle;
	}

	float getEvaluation() const {
		return evaluation;
	}

	float getOriginalEvaluation() const {
		return originalEvaluation;
	}

	Genes getChromosome() const {
		return chromosome;
	}

	Path getPath() const {
		return path;
	}

	bool getIsChild() const {
		return isChild;
	}

	Coords getCollisionPoint() const {
		return collisionPoint;
	}

	int getCrashLineIdx() const {
		return crashLineIdx;
	}

	void setShuttle(const Shuttle& shuttle) { this->shuttle = shuttle; }
	void setEvaluation(float evaluation) { this->evaluation = evaluation; }
	void setOriginalEvaluation(float originalEvaluation) { this->originalEvaluation = originalEvaluation; }
	void setChromosome(const Genes& chromosome) { this->chromosome = chromosome; }
	void setPath(const Path& path) { this->path = path; }
	void setIsChild(bool isChild) { this->isChild = isChild; }
	void setCollisionPoint(const Coords& collisionPoint) { this->collisionPoint = collisionPoint; }
	void setCrashLineIdx(bool crashLineIdx) { this->crashLineIdx = crashLineIdx; }

	bool operator<(const Chromosome& chromosome) const;
	Chromosome& operator=(const Chromosome& other);

	void evaluate(Surface* surface);
	void addGene(const Gene& gene);
	Gene getGene(int geneIdx) const;
	void simulate(Surface* surface, bool& goodForLanding);
	void mutate();
	bool isValid();

	string constructSVGData(const SVGManager& svgManager) const;

private:
	Shuttle shuttle;
	float evaluation;
	float originalEvaluation;
	bool isChild;
	Coords collisionPoint;
	int crashLineIdx;

	Genes chromosome;
	Path path;
};

//*************************************************************************************************************
//*************************************************************************************************************

Chromosome::Chromosome() :
	shuttle(),
	evaluation(0.f),
	originalEvaluation(0.f),
	chromosome(),
	path(),
	isChild(false),
	collisionPoint(),
	crashLineIdx(INVALID_ID)
{

}

//*************************************************************************************************************
//*************************************************************************************************************

Chromosome::~Chromosome() {
	chromosome.clear();
	path.clear();
}

//*************************************************************************************************************
//*************************************************************************************************************

bool Chromosome::operator<(const Chromosome& chromosome) const {
	return evaluation < chromosome.evaluation;
}

//*************************************************************************************************************
//*************************************************************************************************************

Chromosome& Chromosome::operator=(const Chromosome& other) {
	if (this != &other) {
		chromosome.clear();
		path.clear();

		shuttle = other.shuttle;
		evaluation = other.evaluation;
		chromosome = other.chromosome;
		path = other.path;
		isChild = other.isChild;
	}
	return *this;
}

//*************************************************************************************************************
//*************************************************************************************************************

string Chromosome::constructSVGData(const SVGManager& svgManager) const {
	string svgStr = "";

	svgStr.append(GROUP_BEGIN);
	svgStr.append(NEW_LINE);

	vector<string> titleLines;
	shuttle.getTitleLines(titleLines);
	string evaluationLine = "Evaluation: ";
	evaluationLine.append(to_string(evaluation));
	titleLines.push_back(evaluationLine);
	string title = svgManager.constructMultiLineTitle(titleLines);
	svgStr.append(title);
	svgStr.append(NEW_LINE);
	svgStr.append(NEW_LINE);

	svgStr.append(POLYLINE_BEGIN);

	for (size_t positionIdx = 0; positionIdx < path.size() - 1; ++positionIdx) {
		Coords position = path[positionIdx];
		Coords nextPosition = path[positionIdx + 1];

		Coord startX = position.getXCoord();
		Coord startY = MAP_HEIGHT - position.getYCoord();
		Coord endX = nextPosition.getXCoord();
		Coord endY = MAP_HEIGHT - nextPosition.getYCoord();

		svgStr.append(to_string(startX));
		svgStr.append(",");
		svgStr.append(to_string(startY));
		svgStr.append(" ");
		svgStr.append(to_string(endX));
		svgStr.append(",");
		svgStr.append(to_string(endY));
		svgStr.append(" ");
	}
	svgStr.append("\" ");

	svgStr.append(STYLE_BEGIN);
	svgStr.append(FILL_NONE);

	string strokeRGB = svgManager.constructStrokeForRGB(255, int(255.f * evaluation), int(255.f * evaluation));
	if (isChild) {
		strokeRGB = svgManager.constructStrokeForRGB(0, 255, 0);
	}
	svgStr.append(strokeRGB);
	svgStr.append(";");

	string strokeWidth = svgManager.constructStrokeWidth(10);
	svgStr.append(strokeWidth);
	svgStr.append(ATTRIBUTE_END);

	svgStr.append(ONCLICK_DISPLAY_DATA);

	svgStr.append(POLYLINE_END);
	svgStr.append(GROUP_END);
	svgStr.append(NEW_LINE);
	svgStr.append(NEW_LINE);

	return svgStr;
}

//*************************************************************************************************************
//*************************************************************************************************************

void Chromosome::evaluate(Surface* surface) {
	float dist = surface->findDistanceToLandingZone(collisionPoint, crashLineIdx);
	float hSpeed = abs(shuttle.getHSpeed());
	float vSpeed = abs(shuttle.getVSpeed());
	float currentSpeed = sqrt((hSpeed * hSpeed) + (vSpeed * vSpeed));
	
	if (dist > 0.f) {
		evaluation = MAX_OUT_OF_LANDING_AREA_SCORE -
			(MAX_OUT_OF_LANDING_AREA_SCORE * (dist / surface->getMaxDistance()));

		float speedPen = SPEED_PENALTY_WEIGHT *
			max(currentSpeed - MAX_OUT_OF_LANDING_AREA_SCORE, 0.f);
		evaluation -= speedPen;
	}
	else if (hSpeed > MAX_H_ABS_SPEED || vSpeed > MAX_V_ABS_SPEED) {
		float hPen = 0.f;
		if (MAX_H_ABS_SPEED < hSpeed) {
			hPen = (hSpeed - MAX_H_ABS_SPEED) / 2;
		}

		float vPen = 0.f;
		if (MAX_V_ABS_SPEED < vSpeed) {
			vPen = (vSpeed - MAX_V_ABS_SPEED) / 2;
		}

		evaluation = MAX_LANDING_AREA_BIG_SPEED_SCORE - hPen - vPen;
	}
	else {
		evaluation = MAX_LANDING_AREA_BIG_SPEED_SCORE +
			(MAX_OUT_OF_LANDING_AREA_SCORE * (shuttle.getFuel() / shuttle.getInitialFuel()));
	}

	//evaluation = dist;	
}

//*************************************************************************************************************
//*************************************************************************************************************

void Chromosome::addGene(const Gene& gene) {
	chromosome.push_back(gene);
}

//*************************************************************************************************************
//*************************************************************************************************************

Gene Chromosome::getGene(int geneIdx) const {
	return chromosome[geneIdx];
}

//*************************************************************************************************************
//*************************************************************************************************************

void Chromosome::simulate(Surface* surface, bool& goodForLanding) {
	path.clear();

	shuttle.setInitialFuel(shuttle.getFuel());
	for (size_t geneIdx = 0; geneIdx < chromosome.size(); ++geneIdx) {
		Gene* gene = &chromosome[geneIdx];
		shuttle.simulate(gene->rotate, gene->power);
		path.push_back(shuttle.getPosition());

		if (shuttle.goodForLanding(surface->getLandingZone())) {
			chromosome.erase(chromosome.begin() + geneIdx, chromosome.end());
			chromosome.push_back(Gene(0, 0));
			goodForLanding = true;
			break;
		}

		if (0 < geneIdx) {
			const Coords& lastPosition = path[geneIdx - 1];
			surface->collisionWithSurface(lastPosition, shuttle.getPosition(), collisionPoint, crashLineIdx);

			if (collisionPoint.isValid()) {
				path[path.size() - 1] = collisionPoint;
				break;
			}
		}
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void Chromosome::mutate() {
	int randGeneIdx = rand() % chromosome.size();
	chromosome[randGeneIdx].rotate = (rand() % (MAX_ROTATION_ANGLE * 2)) - MAX_ROTATION_ANGLE;
	chromosome[randGeneIdx].power = (rand() % (MAX_POWER * 2)) - MAX_POWER;
}

//*************************************************************************************************************
//*************************************************************************************************************

bool Chromosome::isValid() {
	return chromosome.size() > 0;
}

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

typedef vector<Chromosome> Chromosomes;

class GeneticPopulation {
public:
	GeneticPopulation();
	~GeneticPopulation();

	Chromosomes getPopulation() const {
		return population;
	}

	Surface getSurface() const {
		return surface;
	}

	float getEvaluationsSum() const {
		return evaluationsSum;
	}

	void setSurface(const Surface& surface) { this->surface = surface; }
	void setChromosomes(const Chromosomes& population) { this->population = population; }
	void setEvaluationsSum(float evaluationsSum) { this->evaluationsSum = evaluationsSum; }

	void initRandomPopulation();
	bool simulate(Shuttle* shuttle, Chromosome& solutionChromosome);
	void arrangeChromosomesByEvaluation();
	void selectParentsForChild(
		const Chromosome** parent0,
		const Chromosome** parent1
	) const;
	void makeChildren(Chromosomes& children);

	void crossover(
		const Chromosome& parent0,
		const Chromosome& parent1,
		Chromosome& child0,
		Chromosome& child
	);

	void resetChildFlags();
	void makeNextGeneration();
	int selectParent(float randomFloat) const;
	void reset();
	void crossoverGenes(
		const Gene& parent0Gene,
		const Gene& parent1Gene,
		Gene& childGene
	) const;

	void mutateGene(
		float parent0OriginalEvaluation,
		float parent1OriginalEvaluation,
		int geneIdx,
		Gene& gene
	) const;

	string constructSVGData(const SVGManager& svgManager) const;

private:
	Chromosomes population;
	Surface surface;
	float evaluationsSum;
};

//*************************************************************************************************************
//*************************************************************************************************************

GeneticPopulation::GeneticPopulation() :
	population(POPULATION_SIZE),
	surface(),
	evaluationsSum(0.f)
{
}

//*************************************************************************************************************
//*************************************************************************************************************

GeneticPopulation::~GeneticPopulation() {
}

//*************************************************************************************************************
//*************************************************************************************************************

void GeneticPopulation::initRandomPopulation() {
	int maxAngleRand = (2 * MAX_ROTATION_ANGLE_STEP) + 1;
	//int maxPowerRand = (2 * MAX_POWER_STEP) + 1;
	int maxPowerRand = MAX_POWER + 1;

	random_device angleRd; // obtain a random number from hardware
	mt19937 angleEng(angleRd()); // seed the generator
	uniform_int_distribution<> rotateDistr(0, maxAngleRand - 1);

	random_device powerRd;
	mt19937 powerEng(powerRd());
	uniform_int_distribution<> powerDistr(MIN_POWER, MAX_POWER);

	int currentAngle = 0;

	for (size_t chromIdx = 0; chromIdx < population.size(); ++chromIdx) {
		for (int geneIdx = 0; geneIdx < CHROMOSOME_SIZE; ++geneIdx) {

			int randAngle = 0;
			int randPower = 0;

			if (USE_OLDSCHOOL_RAND) {
				randAngle = rand() % maxAngleRand;
				randPower = rand() % maxPowerRand;
			}
			else {
				randAngle = rotateDistr(angleEng);
				randPower = powerDistr(powerEng);
			}

			randAngle += MIN_ROTATION_ANGLE_STEP;
			currentAngle += randAngle;
			currentAngle = max(MIN_ROTATION_ANGLE, min(currentAngle, MAX_ROTATION_ANGLE));

			Gene gene(currentAngle, randPower);
			population[chromIdx].addGene(gene);

#ifdef SIMULATION_OUTPUT
			//cout << currentAngle << ", " << randPower << endl;
#endif // SIMULATION_OUTPUT
		}
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

bool GeneticPopulation::simulate(Shuttle* shuttle, Chromosome& solutionChromosome) {
	bool foundResChromosome = false;

	for (size_t chromIdx = 0; chromIdx < population.size(); ++chromIdx) {
		Chromosome* chromosome = &population[chromIdx];
		bool chromosomePosAlredyCalced = chromosome->getShuttle().getPosition().isValid();

		if (!chromosome->getIsChild() && chromosomePosAlredyCalced) {
			continue;
		}

		chromosome->setShuttle(*shuttle);
		chromosome->simulate(&surface, foundResChromosome);

		if (foundResChromosome) {
			solutionChromosome = *chromosome;
			break;
		}

		chromosome->evaluate(&surface);
		evaluationsSum += chromosome->getEvaluation();
	}

	return foundResChromosome;
}

//*************************************************************************************************************
//*************************************************************************************************************

void GeneticPopulation::arrangeChromosomesByEvaluation() {
	for (size_t chromIdx = 0; chromIdx < population.size(); ++chromIdx) {
		Chromosome& chromosome = population[chromIdx];
		const float& originalEvaluation = chromosome.getEvaluation();
		chromosome.setOriginalEvaluation(originalEvaluation);
		float normEvaluation = originalEvaluation / evaluationsSum;
		chromosome.setEvaluation(normEvaluation);
	}

	sort(population.rbegin(), population.rend());

	for (size_t chromIdx = 0; chromIdx < population.size(); ++chromIdx) {
		Chromosome& chromosome = population[chromIdx];
		if (0 < chromIdx) {
			float prevEvaluation = population[chromIdx - 1].getEvaluation();
			chromosome.setEvaluation(prevEvaluation + chromosome.getEvaluation());
		}
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

int GeneticPopulation::selectParent(float randomFloat) const {
	int parentChromIdx = static_cast<int>(population.size() - 1);

	for (size_t chromIdx = 0; chromIdx < population.size(); ++chromIdx) {
		if (randomFloat < population[chromIdx].getEvaluation()) {
			parentChromIdx = chromIdx;
			break;
		}
	}

	return parentChromIdx;
}

//*************************************************************************************************************
//*************************************************************************************************************

void GeneticPopulation::reset() {
	evaluationsSum = 0.f;
}

//*************************************************************************************************************
//*************************************************************************************************************

void GeneticPopulation::crossoverGenes(
	const Gene& parent0Gene,
	const Gene& parent1Gene,
	Gene& childGene
) const {
	float randomfloat = Math::randomFloatBetween0and1();
	childGene.rotate =
		parent0Gene.rotate +
		static_cast<int>(randomfloat * (parent1Gene.rotate - parent0Gene.rotate));

	randomfloat = Math::randomFloatBetween0and1();
	childGene.power =
		parent0Gene.power +
		static_cast<int>(randomfloat * (parent1Gene.power - parent0Gene.power));
}

//*************************************************************************************************************
//*************************************************************************************************************

void GeneticPopulation::mutateGene(
	float parent0OriginalEvaluation,
	float parent1OriginalEvaluation,
	int geneIdx,
	Gene& gene
) const {
	int mudblood = 5;
	const float maxScore = max(parent0OriginalEvaluation, parent1OriginalEvaluation);

	if (MAX_OUT_OF_LANDING_AREA_SCORE < maxScore) {
		mudblood = 3;
	}

	if (MAX_LANDING_AREA_BIG_SPEED_SCORE < maxScore) {
		mudblood = 1;
	}

	const float progress = (static_cast<float>(geneIdx)) / CHROMOSOME_SIZE;
	const float progressChance = .4f + 1.f * progress;
	const float mutationChance = .02f * mudblood * progressChance;

	const float randomFloat = Math::randomFloatBetween0and1();
	if (randomFloat < mutationChance) {
		gene.rotate += (rand() % 21) - 10;
		gene.power += (rand() % 2) - 1;
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void GeneticPopulation::selectParentsForChild(
	const Chromosome** parent0,
	const Chromosome** parent1
) const {
	float randomFloat = Math::randomFloatBetween0and1();

	const int parent0Idx = selectParent(randomFloat);

	randomFloat = Math::randomFloatBetween0and1();
	int parent1Idx = selectParent(randomFloat);

	*parent0 = &population[parent0Idx];
	*parent1 = &population[parent1Idx];
}

//*************************************************************************************************************
//*************************************************************************************************************

void GeneticPopulation::makeChildren(Chromosomes& children) {
	for (int childIdx = 0; childIdx < CHILDREN_COUNT; ++childIdx) {
		const Chromosome* parent0 = nullptr;
		const Chromosome* parent1 = nullptr;
		selectParentsForChild(&parent0, &parent1);

		//!! Check if returned value is better than local and reference
		Chromosome child0;
		Chromosome child1;

		//!! Measure the dereferencing of the pointer
		crossover(*parent0, *parent1, child0, child1);
		children.push_back(child0);
		children.push_back(child1);
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void GeneticPopulation::crossover(
	const Chromosome& parent0,
	const Chromosome& parent1,
	Chromosome& child0,
	Chromosome& child1
) {
	const float& parent0OriginalEvaluation = parent0.getOriginalEvaluation();
	const float& parent1OriginalEvaluation = parent1.getOriginalEvaluation();

	for (int geneIdx = 0; geneIdx < CHROMOSOME_SIZE; ++geneIdx) {
		Gene& parent0Gene = parent0.getGene(geneIdx);
		Gene& parent1Gene = parent1.getGene(geneIdx);

		//Gene childGene;
		//crossoverGenes(parent0Gene, parent1Gene, childGene);
		//mutateGene(parent0OriginalEvaluation, parent1OriginalEvaluation, geneIdx, childGene);
		//child0.addGene(childGene);
		//
		//crossoverGenes(parent0Gene, parent1Gene, childGene);
		//mutateGene(parent0OriginalEvaluation, parent1OriginalEvaluation, geneIdx, childGene);
		//child1.addGene(childGene);


		mutateGene(parent0OriginalEvaluation, parent1OriginalEvaluation, geneIdx, parent0Gene);
		mutateGene(parent0OriginalEvaluation, parent1OriginalEvaluation, geneIdx, parent1Gene);

		float randomfloat = Math::randomFloatBetween0and1();
		if (randomfloat >= CROSSOVER_GENE_PROB) {
			child0.addGene(parent0Gene);
			child1.addGene(parent1Gene);
		}
		else {
			child0.addGene(parent1Gene);
			child1.addGene(parent0Gene);
		}
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void GeneticPopulation::resetChildFlags() {
	for (size_t chromeIdx = 0; chromeIdx < population.size(); ++chromeIdx) {
		population[chromeIdx].setIsChild(false);
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void GeneticPopulation::makeNextGeneration() {
	//resetChildFlags();

	Chromosomes children;
	makeChildren(children);

	population.clear();
	population = children;
}

//*************************************************************************************************************
//*************************************************************************************************************

string GeneticPopulation::constructSVGData(const SVGManager& svgManager) const {
#ifdef SVG
	string svgStr = "";

	for (size_t chromeIdx = 0; chromeIdx < population.size(); ++chromeIdx) {
		string chromeSVGData = population[chromeIdx].constructSVGData(svgManager);
		svgStr.append(chromeSVGData);
	}

	return svgStr;
#endif // SVG

	return "";
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
	void gameEnd();

	void debug() const;

private:
	int turnsCount;

	Shuttle* shuttle;
	Surface* surface;

	GeneticPopulation geneticPopulation;
	Chromosome solutionChromosome;

#ifdef SVG
	SVGManager svgManager;
#endif // SVG

};

//*************************************************************************************************************
//*************************************************************************************************************

Game::Game() :
	turnsCount(0),
	shuttle(NULL),
	surface(NULL),
	geneticPopulation(),
	solutionChromosome()
#ifdef SVG
	,svgManager()
#endif // SVG
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
	geneticPopulation.setSurface(*surface);
	geneticPopulation.initRandomPopulation();

#ifdef SVG
	string surfaceSVGData = surface->constructSVGData(svgManager);
	svgManager.filePrintStr(surfaceSVGData);
#endif // SVG
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::gameLoop() {
	while (true) {
		getTurnInput();
		turnBegin();
		makeTurn();
		turnEnd();

		break;
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::getGameInput() {
	int surfaceN; // the number of points used to draw the surface of Mars.
	cin >> surfaceN;

	if (OUTPUT_GAME_DATA) {
		cerr << surfaceN << endl;
	}

	Coords point0;

	float leftDistToLandingZone = 0.f;
	float rightDistToLandingZone = 0.f;

	float* distToLandingZone = &leftDistToLandingZone;

	bool landingZoneFound = false;
	int landingZoneDirection = LZD_RIGHT;

	for (int i = 0; i < surfaceN; i++) {
		int landX; // X coordinate of a surface point. (0 to 6999)
		int landY; // Y coordinate of a surface point. By linking all the points together in a sequential fashion, you form the surface of Mars.
		cin >> landX >> landY; cin.ignore();

		if (OUTPUT_GAME_DATA) {
			cerr << landX << " " << landY << endl;
		}

		if (landingZoneFound && 0.f == rightDistToLandingZone) {
			landingZoneDirection = LZD_LEFT;
			distToLandingZone = &rightDistToLandingZone;
		}

		Coords point1((float)landX, (float)landY);

		if (0 != i) {
			if (point0.getYCoord() != point1.getYCoord()) {
				*distToLandingZone += distance(point0, point1);
			}

			surface->addLine(point0, point1, landingZoneDirection, landingZoneFound);
		}

		point0 = point1;
	}

	if (leftDistToLandingZone > rightDistToLandingZone) {
		surface->setMaxDistance(leftDistToLandingZone);
	}
	else {
		surface->setMaxDistance(rightDistToLandingZone);
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

	cin >> X >> Y >> hSpeed >> vSpeed >> fuel >> rotate >> power; cin.ignore();

	if (OUTPUT_GAME_DATA) {
		cerr << X << " " << Y << " " << hSpeed << " " << vSpeed << " ";
		cerr << fuel << " " << rotate << " " << power << endl;
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
	bool answerFound = false;

	int populationId = 0;
	while (!answerFound) {
		geneticPopulation.reset();
		answerFound = geneticPopulation.simulate(shuttle, solutionChromosome);
		geneticPopulation.arrangeChromosomesByEvaluation();

#ifdef SVG
		string populationSVGData = geneticPopulation.constructSVGData(svgManager);
		string populationIdSVGData = svgManager.constructGId(populationId++);
		svgManager.filePrintStr(populationIdSVGData);
		svgManager.filePrintStr(DISPLAY_NONE);
		svgManager.filePrintStr(ID_END);
		svgManager.filePrintStr(populationSVGData);
		svgManager.filePrintStr(GROUP_END);
		svgManager.filePrintStr(NEW_LINE);
		svgManager.filePrintStr(NEW_LINE);
#endif // SVG

		geneticPopulation.makeNextGeneration();

		if (populationId == MAX_POPULATION) {

#ifdef SIMULATION_OUTPUT
			const Genes& genes = geneticPopulation.getPopulation()[44].getChromosome();
			for (size_t geneIdx = 0; geneIdx < genes.size(); ++geneIdx) {
				const Gene& gene = genes[geneIdx];
				cout << gene.rotate << ", " << gene.power << ","<< endl;
			}
#endif // SIMULATION_OUTPUT

			break;
		}
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::makeTurn() {
	if (solutionChromosome.isValid()) {
		Gene& turnGene = solutionChromosome.getGene(turnsCount);
		cout << turnGene.rotate << " " << turnGene.power << endl;
	}
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
	gameEnd();
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::gameEnd() {
#ifdef SVG
	svgManager.fileDone();
#endif // SVG
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

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {

#ifdef REDIRECT_CIN_FROM_FILE
	ifstream in(INPUT_FILE_NAME);
	streambuf *cinbuf = cin.rdbuf();
	cin.rdbuf(in.rdbuf());
#endif // REDIRECT_CIN_FROM_FILE

#ifdef REDIRECT_COUT_TO_FILE
	ofstream out(OUTPUT_FILE_NAME);
	streambuf *coutbuf = cout.rdbuf();
	cout.rdbuf(out.rdbuf());
#endif // REDIRECT_COUT_TO_FILE

#ifdef TESTS
	doctest::Context context;
	int res = context.run();
#else
	Game game;
	game.play();
#endif // TESTS

	return 0;
}