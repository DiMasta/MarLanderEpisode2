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

//#define SVG
//#define REDIRECT_CIN_FROM_FILE
//#define REDIRECT_COUT_TO_FILE
//#define SIMULATION_OUTPUT
//#define DEBUG_ONE_TURN
//#define USE_UNIFORM_RANDOM
//#define OUTPUT_GAME_DATA

#ifdef SVG
#include "SVGManager.h"
#endif // SVG

using namespace std;

const int MAP_WIDTH = 7000;
const int MAP_HEIGHT = 3000;
const float MAX_DISTANCE = static_cast<float>(sqrt((MAP_WIDTH * MAP_WIDTH) + (MAP_HEIGHT * MAP_HEIGHT)));
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
const float MAX_POSSIBLE_HSPEED = 50.f; // This is from observation it may not be correct
const float MAX_POSSIBLE_VSPEED = 150.f; // This is from observation it may not be correct
const float COMBINED_EVALUATION_WEIGHT = 100.f; // Just for bigger evalaluations, may be it is worng
const float FLOAT_MAX_RAND = static_cast<float>(RAND_MAX);
const float DIST_WEIGHT = 4.f;

const string INPUT_FILE_NAME = "input.txt";
const string OUTPUT_FILE_NAME = "output.txt";

const int CHROMOSOME_SIZE = 100;//300
const int POPULATION_SIZE = 90;
const int MAX_POPULATION = 100;
const int CHILDREN_COUNT = POPULATION_SIZE;
const float ELITISM_RATIO = 0.f; // The perscentage of the best chromosomes to transfer directly to the next population, unchanged, after other operators are done!
const float PROBABILITY_OF_MUTATION = 0.f; // The probability to mutate a gene
const float PROBABILITY_OF_CROSSOVER = 1.f; // The probability to use the new child or transfer the parent directly
const float CORRECT_THE_RANDOM_FOR_SELECTION = 0.5f; // Force the selection of more fit individuals

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
const int LAST_COMMANDS_TO_EDIT = 2;

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

#ifdef USE_UNIFORM_RANDOM
random_device rd;
mt19937 mt(rd());
uniform_real_distribution<float> dist(0.0, 1.0);
#endif

namespace Math {
#ifdef USE_UNIFORM_RANDOM
	float randomFloatBetween0and1() {
		return dist(mt);
	}
#else
	float randomFloatBetween0and1() {
		return static_cast<float>(rand()) / FLOAT_MAX_RAND;
	}
#endif
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

	/// Check if the shuutle has crashed on landing area
	/// @paramp[in] collisionPoint point where the shuttle crashed
	bool crashedOnLandingArea(const Coords& collisionPoint) const;

#ifdef SVG
	string constructSVGData(const SVGManager& svgManager) const;
#endif // SVG

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

				crashLineIdx = static_cast<int>(lineIdx);
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

bool Surface::crashedOnLandingArea(const Coords& collisionPoint) const {
	const Coord x = collisionPoint.getXCoord();
	const Coord y = collisionPoint.getYCoord();

	const Coord landingLeftX = landingZone.getPoint0().getXCoord();
	const Coord landingRightX = landingZone.getPoint1().getXCoord();
	const Coord landingY = landingZone.getPoint1().getYCoord();

	const bool inLandingXZone = (landingLeftX <= x) && (x <= landingRightX);
	const bool inLandingYZone = (landingY - 25.f <= y) && (y <= landingY + 25.f);

	return inLandingXZone && inLandingYZone;
}

//*************************************************************************************************************
//*************************************************************************************************************

#ifdef SVG
string Surface::constructSVGData(const SVGManager& svgManager) const {
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
}
#endif // SVG

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
		float initialSpeed,
		ComponentType componentType,
		float& displacement,
		float& acceleration
	);

	/// Apply new angle change step, considering the ranges for steps and final angle
	/// @param[in] newRotateAngleStep the new rotate angle step, may be out of the range [-15, 15] but it will be clamped
	void applyNewRotateAngle(int newRotateAngleStep);

	/// Apply new power change step, considering the ranges for steps and final power
	/// @param[in] newPowerStep the new power step, may be out of the range [0, 4] but it will be clamped
	void applyNewPower(int newPowerStep);

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
	float initialSpeed,
	ComponentType componentType,
	float& displacement,
	float& acceleration
) {
	int theta = RIGHT_ANGLE + rotate;
	float rad = theta * PI / (2 * RIGHT_ANGLE);

	float mult = cos(rad);
	if (CT_VERTICAL == componentType) {
		mult = sin(rad);
	}

	acceleration = mult * (float)power;

	if (CT_VERTICAL == componentType) {
		acceleration -= MARS_GRAVITY;
	}

	displacement = (float)initialSpeed + (.5f * acceleration);
}

//*************************************************************************************************************
//*************************************************************************************************************

void Shuttle::applyNewRotateAngle(int newRotateAngleStep) {
	// First check if the ange change step is in the range [-15, 15]
	// It may be outside of the range, after a crossoverb
	int newRotateAngleStepClamped = min(max(MIN_ROTATION_ANGLE_STEP, newRotateAngleStep), MAX_ROTATION_ANGLE_STEP);
	int newRotateAngle = rotate + newRotateAngleStepClamped;
	
	// Clamp the new angle in the range [-90, 90]
	rotate = min(max(MIN_ROTATION_ANGLE, newRotateAngle), MAX_ROTATION_ANGLE);
}

//*************************************************************************************************************
//*************************************************************************************************************

void Shuttle::applyNewPower(int newPowerStep) {
	// First check if the power change step is in the range [-1, 1]
	// It may be outside of the range, after a crossover
	int newPowerStepClamped = min(max(MIN_POWER_STEP, newPowerStep), MAX_POWER_STEP);
	int newPower = power + newPowerStepClamped;

	// Clamp the new angle in the range [0, 4]
	power = min(max(MIN_POWER, newPower), MAX_POWER);
}

//*************************************************************************************************************
//*************************************************************************************************************

void Shuttle::simulate(int rotateAngle, int thrustPower) {
	applyNewRotateAngle(rotateAngle);
	applyNewPower(thrustPower);

	fuel -= power;

	float displacementX = 0.f;
	float displacementY = 0.f;

	float accelerationX = 0.f;
	float accelerationY = 0.f;

	calculateComponents(hSpeed, CT_HORIZONTAL, displacementX, accelerationX);
	calculateComponents(vSpeed, CT_VERTICAL, displacementY, accelerationY);

	float newX = (float)position.getXCoord() + displacementX;
	float newY = (float)position.getYCoord() + displacementY;

	float newHSpeed = (float)hSpeed + accelerationX;
	float newVSpeed = (float)vSpeed + accelerationY;

	position.setXCoord(newX);
	position.setYCoord(newY);

	hSpeed = newHSpeed;
	vSpeed = newVSpeed;
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

	int hSpeedRounded = abs(static_cast<int>(round(hSpeed)));
	int vSpeedRounded = abs(static_cast<int>(round(vSpeed)));

	if (shuttleX > landingZoneX0 && shuttleX < landingZoneX1) {
		Coord distToLandingZone = shuttleY - landingZoneY;
		if (distToLandingZone < SAVE_DISTANCE_TO_LANDING_ZONE) {
			if (rotate >= MIN_ROTATION_ANGLE_STEP && rotate <= MAX_ROTATION_ANGLE_STEP) {
				if ((hSpeedRounded <= MAX_H_SPEED_FOR_LANDING) && (vSpeedRounded <= MAX_V_SPEED_FOR_LANDING)) {
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

	void clamp();

	/// The change in angle, based on previous postion, in range [-15, 15]
	int rotate;

	/// The change in power, based on previoud postion, in range [-1, 1]
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

//*************************************************************************************************************
//*************************************************************************************************************

void Gene::clamp() {
	rotate = max(MIN_ROTATION_ANGLE, min(rotate, MAX_ROTATION_ANGLE));
	power = max(MIN_POWER, min(power, MAX_POWER));
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

	Genes getChromosome() const {
		return chromosome;
	}

	Genes getOutputCommands() const {
		return outputCommands;
	}

	Path getPath() const {
		return path;
	}

	Coords getCollisionPoint() const {
		return collisionPoint;
	}

	int getCrashLineIdx() const {
		return crashLineIdx;
	}

	bool getSelected() const {
		return selected;
	}

	void setShuttle(const Shuttle& shuttle) { this->shuttle = shuttle; }
	void setEvaluation(float evaluation) { this->evaluation = evaluation; }
	void setChromosome(const Genes& chromosome) { this->chromosome = chromosome; }
	void setPath(const Path& path) { this->path = path; }
	void setCollisionPoint(const Coords& collisionPoint) { this->collisionPoint = collisionPoint; }
	void setCrashLineIdx(bool crashLineIdx) { this->crashLineIdx = crashLineIdx; }
	void setSelected(bool selected) { this->selected = selected; }

	bool operator<(const Chromosome& chromosome) const;
	Chromosome& operator=(const Chromosome& other);

	void evaluate(Surface* surface);
	void addGene(const Gene& gene);
	Gene getGene(int geneIdx) const;
	void simulate(Surface* surface, bool& goodForLanding);
	void mutate();
	bool isValid();

#ifdef SVG
	string constructSVGData(const SVGManager& svgManager) const;
#endif // SVG

private:
	Shuttle shuttle;
	float evaluation; /// Maybe I could work with integer evaluation !? experiment
	Coords collisionPoint; /// Calculations for the crash point may slitly vary from the platform
	int crashLineIdx;

	Genes chromosome;
	Genes outputCommands; /// Actual commands for the online platform, could be OPTIMIZED when solution is found
	Path path; /// Used to store the path which will be visualized in SVG, could be OPTIMIZED when solution is found

	bool selected; /// Shows if the individual is selected for the next generation, for debug purpose only
};

//*************************************************************************************************************
//*************************************************************************************************************

Chromosome::Chromosome() :
	shuttle(),
	evaluation(0.f),
	chromosome(),
	path(),
	collisionPoint(),
	crashLineIdx(INVALID_ID),
	outputCommands(),
	selected(false)
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
		outputCommands.clear();
		path.clear();

		shuttle = other.shuttle;
		evaluation = other.evaluation;
		collisionPoint = other.collisionPoint;
		crashLineIdx = other.crashLineIdx;
		chromosome = other.chromosome;
		outputCommands = other.outputCommands;
		path = other.path;
		selected = other.selected;
	}
	return *this;
}

//*************************************************************************************************************
//*************************************************************************************************************

#ifdef SVG
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

	string strokeRGB = svgManager.constructStrokeForRGB(255, 255, 255);

	if (selected) {
		strokeRGB = svgManager.constructStrokeForRGB(0, 0, 255);
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
#endif // SVG

//*************************************************************************************************************
//*************************************************************************************************************

void Chromosome::evaluate(Surface* surface) {
	//var currentSpeed = Math.sqrt(Math.pow(this.xspeed, 2) + Math.pow(this.yspeed, 2));
	const float hSpeed = shuttle.getHSpeed();
	const float vSpeed = shuttle.getVSpeed();
	const float currentSpeed = sqrt((hSpeed * hSpeed) + (vSpeed * vSpeed));

	// 0-100: crashed somewhere, calculate score by distance to landing area
	//if (!hitLandingArea) {
	//
	//	var lastX = this.points[this.points.length-2][0];
	//	var lastY = this.points[this.points.length-2][1];
	//	var distance = level.getDistanceToLandingArea(lastX, lastY);
	//
	//	// Calculate score from distance
	//	this.score = 100 - (100 * distance / level.max_dist);
	//
	//	// High speeds are bad, they decrease maneuvrability
	//	var speedPen = 0.1 * Math.max(currentSpeed - 100, 0);
	//	this.score -= speedPen;
	//}
	if (!surface->crashedOnLandingArea(collisionPoint)) {
		const float distance = surface->findDistanceToLandingZone(collisionPoint, crashLineIdx);

		// Calculate score from distance
		evaluation = 100 - (100 * distance / MAX_DISTANCE);

		// High speeds are bad, they decrease maneuvrability
		const float speedPen = 0.1f * max(currentSpeed - 100.f, 0.f);
		evaluation -= speedPen;
	}
	else if (vSpeed < -MAX_V_ABS_SPEED || MAX_H_ABS_SPEED < abs(hSpeed)) {
	//// 100-200: crashed into landing area, calculate score by speed above safety
	//else if (this.yspeed < -40 || 20 < Math.abs(this.xspeed)) {
	//	var xPen = 0;
	//	if (20 < Math.abs(this.xspeed)) {
	//		xPen = (Math.abs(this.xspeed) - 20) / 2
	//	}
	//	var yPen = 0
	//		if (this.yspeed < -40) {
	//			yPen = (-40 - this.yspeed) / 2
	//		}
	//	this.score = 200 - xPen - yPen
	//		return;
	//}
		float xPen = 0.f;
		if (MAX_H_ABS_SPEED < abs(hSpeed)) {
			xPen = (abs(hSpeed) - MAX_V_ABS_SPEED) / 2.f;
		}

		float yPen = 0.f;
		if (MAX_V_ABS_SPEED < abs(vSpeed)) {
			yPen = (-MAX_V_ABS_SPEED - vSpeed) / 2.f;
		}

		evaluation = 200.f - xPen - yPen;
	}
	else {
		//// 200-300: landed safely, calculate score by fuel remaining
		//else {
		//	this.score = 200 + (100 * this.fuel / this.initFuel)
		//}

		evaluation = 200.f + (100.f * shuttle.getFuel() / shuttle.getInitialFuel());
	}

	//float dist = surface->findDistanceToLandingZone(collisionPoint, crashLineIdx);
	//float distPortion = 1.f - (dist / MAX_DISTANCE);
	//distPortion *= DIST_WEIGHT;
	//
	//float angle = static_cast<float>(abs(shuttle.getRotate()));
	//float anglePortion = 1.f - (angle / MAX_ROTATION_ANGLE);
	//
	//// The limit for the H speed must be considered somehow
	//float hSpeed = abs(shuttle.getHSpeed());
	//float hSpeedPortion = 1.f - (hSpeed / MAX_POSSIBLE_HSPEED);
	//
	//// The limit for the H speed must be considered somehow
	//float vSpeed = abs(shuttle.getVSpeed());
	//float vSpeedPortion = 1.f - (vSpeed / MAX_POSSIBLE_VSPEED);
	//
	//// After these are polished, add evaluation for the fuel
	//
	//evaluation = distPortion + anglePortion + hSpeedPortion + vSpeedPortion;
	////evaluation *= COMBINED_EVALUATION_WEIGHT;
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

	for (size_t geneIdx = 0; geneIdx < chromosome.size(); ++geneIdx) {
		Gene* gene = &chromosome[geneIdx];
		shuttle.simulate(gene->rotate, gene->power);
		outputCommands.push_back(Gene(shuttle.getRotate(), shuttle.getPower()));
		path.push_back(shuttle.getPosition());

		if (shuttle.goodForLanding(surface->getLandingZone())) {
			chromosome.erase(chromosome.begin() + geneIdx, chromosome.end());
			goodForLanding = true;
			break;
		}

		if (geneIdx > 0) {
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
	for (Gene& gene : chromosome) {
		const float r = Math::randomFloatBetween0and1();

		if (r < PROBABILITY_OF_MUTATION) {
			// Not sure if this is the best mutation
			// Use the logic from the initRandomPopulation
			// May the restrictions from the previous turn could be taken in account
			const int maxAngleRand = (2 * MAX_ROTATION_ANGLE_STEP) + 1;
			const int maxPowerRand = (2 * MAX_POWER_STEP) + 1;

			int randAngle = rand() % maxAngleRand;
			int randPower = rand() % maxPowerRand;

			randAngle += MIN_ROTATION_ANGLE_STEP;
			randPower += MIN_POWER_STEP;

			gene.rotate = randAngle;
			gene.power = randPower;
		}
	}
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

	int getPopulationId() const {
		return populationId;
	}

	void setSurface(const Surface& surface) { this->surface = surface; }
	void setChromosomes(const Chromosomes& population) { this->population = population; }


	/// Use the Continuos Genetic Algorithm methods to make the new generation TODO: maybe not needed
	/// @param[in/out] svgManager used to make the visual debugging
	void makeNextGeneration(
#ifdef SVG
		SVGManager& svgManager
#endif // SVG
	);

	/// Fill with chromosomes which are containing random changes in power [-1, 1] and angle [-15, 15]
	/// The actual angles which should be outputted at the end are stores in each Chromosome's shuttle (rotate and power memebers)
	void initRandomPopulation();

	/// Select parents' indecies, for crossover, using the roullete wheel technique
	/// @param[out] parent0Idx the first parent's index, which will be used for the crossover
	/// @param[out] parent1Idx the second parent's index, which will be used for the crossover
	void selectParentsIdxs(int& parent0Idx, int& parent1Idx);

	/// Crossover pair of chromosomes to make new pair and add them to the new children
	/// @param[in] parent0Idx the first parent's index, which will be used for the crossover
	/// @param[in] parent1Idx the second parent's index, which will be used for the crossover
	/// @param[out] children the new population of chromosomes in which the new created children will be added
	void crossover(int parent0Idx, int parent1Idx, Chromosomes& children);

	/// Mutate the last two chromosomes in the children array, they are the new from the crossover
	/// @param[in/out] children the new children created from the crossover
	void mutate(Chromosomes& children);

	/// Get the best chromosomes from the current population and pass them unchanged to the next
	/// @param[in] population the current population
	/// @param[out] the new generation
	void elitsm(const Chromosomes& population, Chromosomes& children);

	/// Use the Continuos Genetic Algorithm methods to make the children for the new generation
	/// @param[out] children the new generation children
	void makeChildren(Chromosomes& children);

	/// Simulate all Chromosomes, using the commands from each gene to move the given shuttle
	/// @param[in] shuttle the shuttle, for which the simulation is done TODO: do not use pointer
	/// @param[out] solutionCommands the solution commands
	bool simulate(Shuttle* shuttle, Genes& solutionCommands);

	/// Prepare the population for the roullete wheel selection:
	///		- calc the sum of evaluations
	///		- normalize the evalutions
	///		- sort the population's chromosome based on the cumulative sum
	///		- calc the cumulative sum
	void prepareForRoulleteWheel();

	/// Reset stats for each induvidual to the default values
	void reset();

#ifdef SVG
	/// Print the information for the current generation for visual debugging
	/// @param[in/out] svgManager the manager resposible for the visual debugging
	void visualDebugGeneration(SVGManager& svgManager) const;

	/// Fill the visula debug information
	string constructSVGData(const SVGManager& svgManager) const;
#endif // SVG

private:
	Chromosomes population;
	Surface surface;

	int populationId; /// For visual debug purposes
};

//*************************************************************************************************************
//*************************************************************************************************************

GeneticPopulation::GeneticPopulation() :
	population(POPULATION_SIZE),
	surface(),
	populationId(0)
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
	int maxPowerRand = (2 * MAX_POWER_STEP) + 1;

	for (size_t chromIdx = 0; chromIdx < population.size(); ++chromIdx) {
		for (int geneIdx = 0; geneIdx < CHROMOSOME_SIZE; ++geneIdx) {

			int randAngle = rand() % maxAngleRand;
			int randPower = rand() % maxPowerRand;

			randAngle += MIN_ROTATION_ANGLE_STEP;
			randPower += MIN_POWER_STEP;

			Gene gene(randAngle, randPower);
			population[chromIdx].addGene(gene);
		}
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

bool GeneticPopulation::simulate(Shuttle* shuttle, Genes& solutionCommands) {
	bool foundResChromosome = false;

	for (size_t chromIdx = 0; chromIdx < population.size(); ++chromIdx) {
		Chromosome& chromosome = population[chromIdx];

		// TODO: Better way to check for parent
		if (chromosome.getCollisionPoint().isValid()) {
			// Directly transfered parent
			chromosome.evaluate(&surface); // Reset evaluation, because it was modified to fit the roullete wheel
			continue;
		}

		chromosome.setShuttle(*shuttle);
		chromosome.simulate(&surface, foundResChromosome);

		if (foundResChromosome) {
			solutionCommands = chromosome.getOutputCommands();
			break;
		}

		chromosome.evaluate(&surface);
	}

	return foundResChromosome;
}

//*************************************************************************************************************
//*************************************************************************************************************

void GeneticPopulation::selectParentsIdxs(int& parent0Idx, int& parent1Idx) {
	float r = Math::randomFloatBetween0and1();
	r += CORRECT_THE_RANDOM_FOR_SELECTION;

	const int populationSize = static_cast<int>(population.size());
	parent0Idx = populationSize - 1; // If r is too small, the comparison won't pass, so use the last chromosome
	for (int chromIdx = 1; chromIdx < populationSize; ++chromIdx) {
		if (r > population[chromIdx].getEvaluation()) {
			parent0Idx = chromIdx - 1;
			break;
		}
	}

	parent1Idx = parent0Idx;

	while (parent1Idx == parent0Idx) {
		r = Math::randomFloatBetween0and1();
		r += CORRECT_THE_RANDOM_FOR_SELECTION;

		// Code duplication, at the moment I cannot think of more elegant layout
		for (int chromIdx = 1; chromIdx < populationSize; ++chromIdx) {
			if (r > population[chromIdx].getEvaluation()) {
				parent1Idx = chromIdx - 1;
				break;
			}
		}
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void GeneticPopulation::crossover(int parent0Idx, int parent1Idx, Chromosomes& children) {
	const Chromosome& parent0 = population[parent0Idx];
	const Chromosome& parent1 = population[parent1Idx];

	Chromosome child0;
	Chromosome child1;

	for (int geneIdx = 0; geneIdx < CHROMOSOME_SIZE; ++geneIdx) {
		const float beta = Math::randomFloatBetween0and1();
		const float parent0Rotate = static_cast<float>(parent0.getGene(geneIdx).rotate);
		const float parent1Rotate = static_cast<float>(parent1.getGene(geneIdx).rotate);
		const float parent0Power = static_cast<float>(parent0.getGene(geneIdx).power);
		const float parent1Power = static_cast<float>(parent1.getGene(geneIdx).power);

		float child0Rotation = (beta * parent0Rotate) + ((1.f - beta) * parent1Rotate);
		float child1Rotation = ((1.f - beta) * parent0Rotate) + (beta * parent1Rotate);
		float child0Power = (beta * parent0Power) + ((1.f - beta) * parent1Power);
		float child1Power = ((1.f - beta) * parent0Power) + (beta * parent1Power);

		Gene child0Gene;
		child0Gene.rotate = static_cast<int>(round(child0Rotation));
		child0Gene.power = static_cast<int>(round(child0Power));

		Gene child1Gene;
		child1Gene.rotate = static_cast<int>(round(child1Rotation));
		child1Gene.power = static_cast<int>(round(child1Power));

		child0.addGene(child0Gene);
		child1.addGene(child1Gene);
	}

	float r = Math::randomFloatBetween0and1();
	if (r <= PROBABILITY_OF_CROSSOVER) {
		children.push_back(child0);
	}
	else {
		children.push_back(parent0);
	}

	r = Math::randomFloatBetween0and1();
	if (r <= PROBABILITY_OF_CROSSOVER) {
		children.push_back(child1);
	}
	else {
		children.push_back(parent1);
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void GeneticPopulation::mutate(Chromosomes& children) {
	const int childrenSize = static_cast<int>(children.size());
	// Mutate last two chromosomes
	for (int chromIdx = 0; chromIdx < 2; ++chromIdx) {
		Chromosome& chrom = children[childrenSize - chromIdx - 1];
		chrom.mutate();
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void GeneticPopulation::elitsm(const Chromosomes& population, Chromosomes& children) {
	const int elitsCount = static_cast<int>(round(static_cast<float>(population.size()) * ELITISM_RATIO));

	// Chromosomes are sorted in descending order so the best elits are in the begining of the population
	for (int elitIdx = 0; elitIdx < elitsCount; ++elitIdx) {
		children[elitIdx] = population[elitIdx];
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void GeneticPopulation::makeChildren(Chromosomes& children) {
	// While the new population is not completly filled
	// select a pair of parents
	// crossover those parents using the Continuos Genetic Algorithm technique
	// mutate them using the Continuos Genetic Algorithm technique
	while (children.size() < CHILDREN_COUNT) {
		int parent0Idx = INVALID_ID; // For safety reasons
		int parent1Idx = INVALID_ID; // For safety reasons

		selectParentsIdxs(parent0Idx, parent1Idx);

		// Debug
		population[parent0Idx].setSelected(true);
		population[parent1Idx].setSelected(true);

		// Maybe here a check if a valid pair of parents indecies is selected
		crossover(parent0Idx, parent1Idx, children);

		mutate(children);
	}
}

//*************************************************************************************************************
//*************************************************************************************************************


void GeneticPopulation::makeNextGeneration(
#ifdef SVG
	SVGManager& svgManager
#endif // SVG
) {
	reset();
	prepareForRoulleteWheel();

	Chromosomes children;
	makeChildren(children);

	// Apply elitism, get the best chromosomes from the population and overwrite some children
	elitsm(population, children);

#ifdef SVG
	visualDebugGeneration(svgManager);
#endif // SVG

	++populationId;
	population.clear();
	population = children;
}


//*************************************************************************************************************
//*************************************************************************************************************

void GeneticPopulation::prepareForRoulleteWheel() {
	// sum of evaluations
	float sum = 0.f;
	for (const Chromosome& chrom : population) {
		sum += chrom.getEvaluation();
	}

	// normalize the evalutions
	for (Chromosome& chrom : population) {
		chrom.setEvaluation(chrom.getEvaluation() / sum);
	}

	// sort the population's chromosome based on the cumulative sum
	sort(population.rbegin(), population.rend());

	// calc the cumulative sum
	// TODO: first chromosome cumulative evalution could be written 1, left it to make check
	for (size_t chromIdx = 0; chromIdx < population.size(); ++chromIdx) {
		for (size_t nextChromIdx = chromIdx + 1; nextChromIdx < population.size(); ++nextChromIdx) {
			Chromosome& currentChrom = population[chromIdx];
			currentChrom.setEvaluation(currentChrom.getEvaluation() + population[nextChromIdx].getEvaluation());
		}
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void GeneticPopulation::reset() {
	for (Chromosome& chromosome : population) {
		chromosome.setSelected(false);
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

#ifdef SVG
void GeneticPopulation::visualDebugGeneration(SVGManager& svgManager) const {

	string populationSVGData = constructSVGData(svgManager);
	string populationIdSVGData = svgManager.constructGId(populationId);
	svgManager.filePrintStr(populationIdSVGData);
	svgManager.filePrintStr(DISPLAY_NONE);
	svgManager.filePrintStr(ID_END);
	svgManager.filePrintStr(populationSVGData);
	svgManager.filePrintStr(GROUP_END);
	svgManager.filePrintStr(NEW_LINE);
	svgManager.filePrintStr(NEW_LINE);
}

//*************************************************************************************************************
//*************************************************************************************************************

string GeneticPopulation::constructSVGData(const SVGManager& svgManager) const {
	string svgStr = "";

	for (size_t chromeIdx = 0; chromeIdx < population.size(); ++chromeIdx) {
		const Chromosome& chromosome = population[chromeIdx];
		if (chromosome.getShuttle().getPosition().isValid()) {
			string chromeSVGData = chromosome.constructSVGData(svgManager);
			svgStr.append(chromeSVGData);
		}
	}

	return svgStr;
}
#endif // SVG

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

class Game {
public:
	Game();
	~Game();

	void setSolutionCommands(const Genes& solutionCommands) {
		this->solutionCommands = solutionCommands;
	}

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

	/// Edit couple of last commands to make sure the shuttle is vertical
	/// Last commands may differ from my calculations and the online platform
	void postProcessSolutionCommands();

	void debug() const;

private:
	int turnsCount;

	Shuttle* shuttle;
	Surface* surface;

	GeneticPopulation geneticPopulation;

	/// Commands to output on the online platform
	Genes solutionCommands;

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
	solutionCommands()
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

#ifdef DEBUG_ONE_TURN
		break;
#endif // DEBUG_ONE_TURN
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::getGameInput() {
	int surfaceN; // the number of points used to draw the surface of Mars.
	cin >> surfaceN;

#ifdef OUTPUT_GAME_DATA
		cerr << surfaceN << endl;
#endif // OUTPUT_GAME_DATA

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

#ifdef OUTPUT_GAME_DATA
			cerr << landX << " " << landY << endl;
#endif // OUTPUT_GAME_DATA

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

#ifdef OUTPUT_GAME_DATA
		cerr << X << " " << Y << " " << hSpeed << " " << vSpeed << " ";
		cerr << fuel << " " << rotate << " " << power << endl;
#endif // OUTPUT_GAME_DATA

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
	// Run the genetic algorithm only once
	if (turnsCount > 0) {
		return;
	}

	bool answerFound = false;

	while (!answerFound) {
		answerFound = geneticPopulation.simulate(shuttle, solutionCommands);

		if (answerFound) {
			postProcessSolutionCommands();

#ifdef SIMULATION_OUTPUT
			// Commands to directly debug on the online platform
			for (size_t geneIdx = 0; geneIdx < solutionCommands.size(); ++geneIdx) {
				cout << solutionCommands[geneIdx].rotate << ", " << solutionCommands[geneIdx].power << "," << endl;
			}
#endif // SIMULATION_OUTPUT

			break;
		}


		if (geneticPopulation.getPopulationId() == MAX_POPULATION) {
#ifdef SIMULATION_OUTPUT
			// Commands to directly debug on the online platform, for teh bes tindividual, not the solution
			const Genes& bestChromosome = geneticPopulation.getPopulation().front().getOutputCommands();
			for (size_t geneIdx = 0; geneIdx < bestChromosome.size(); ++geneIdx) {
				cout << bestChromosome[geneIdx].rotate << ", " << bestChromosome[geneIdx].power << "," << endl;
			}
#endif // SIMULATION_OUTPUT
			break;
		}
		else {
			geneticPopulation.makeNextGeneration(
#ifdef SVG
				svgManager
#endif // SVG
			);
		}
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::makeTurn() {
	if (solutionCommands.size() > 0) {
		const Gene& command = solutionCommands[turnsCount];
		cout << command.rotate << " " << command.power << endl;
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

void Game::postProcessSolutionCommands() {
	size_t lastGeneIdx = solutionCommands.size() - 1;

	for (size_t commandIdx = lastGeneIdx - LAST_COMMANDS_TO_EDIT + 1; commandIdx <= lastGeneIdx; ++commandIdx) {
		Gene& command = solutionCommands[commandIdx];
		command.rotate = 0;
		command.power = MAX_POWER;
	}

	// Add several safty commands
	solutionCommands.push_back(Gene(0, MAX_POWER));
	solutionCommands.push_back(Gene(0, MAX_POWER));
	solutionCommands.push_back(Gene(0, MAX_POWER));
	solutionCommands.push_back(Gene(0, MAX_POWER));
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