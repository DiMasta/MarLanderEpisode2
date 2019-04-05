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
#define REDIRECT_CIN_FROM_FILE
#define REDIRECT_COUT_TO_FILE
#define SIMULATION_OUTPUT
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
//const float MAX_DISTANCE = 5088.6463f; /// distance from most left to the left point of the landing zone
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

const int CHROMOSOME_SIZE = 200;//300;
const int POPULATION_SIZE = 100;
const int MAX_POPULATION = 500;//250;
const float ELITISM_RATIO = 0.2f; // The perscentage of the best chromosomes to transfer directly to the next population, unchanged, after other operators are done!
const float PROBABILITY_OF_MUTATION = 0.01f; // The probability to mutate a gene
const float PROBABILITY_OF_CROSSOVER = 1.f; // The probability to use the new child or transfer the parent directly

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
const int LAST_COMMANDS_TO_EDIT = 1;
const int ADDITIONAL_TURNS = 4;
const int CHECK_FOR_CRASH_AFTER_GENE = 15;
const int MAX_LINES = 30;

const unsigned int CRASHED_IDX_MASK = 0b1111'1000'0000'0000'0000'0000'0000'0000;
const int CRASHED_IDX_MASK_OFFSET = 27;
const int SELECTED_FLAG = 1;						// 1
const int SOLUTION_FLAG = 1 << 1;					// 2
const int CRASHED_ON_LANDING_ZONE_FLAG = 1 << 2;	// 4
const int CRASHED_FLAG = 1 << 4;					// 8
const int COPIED_FLAG = 1 << 5;						// 16

///Global variables
int INITIAL_FUEL = 0; // Use static member for the initual fuel it is used only in Chromosome::evaluate

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

	inline Coord getXCoord() const {
		return xCoord;
	}

	inline Coord getYCoord() const {
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

	inline Coords getPoint0() const {
		return point0;
	}

	inline Coords getPoint1() const {
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

class Surface {
public:
	Surface();
	~Surface();

	int getLinesCount() const;

	void setLinesCount(int linesCount) {
		this->linesCount = linesCount;
	}

	/// Check if the given line described with two points crosses with a line from the Mars surface
	/// @param[in] point0 the first point of the line to check
	/// @param[in] point1 the second point of the line to check
	/// @param[out] crashedInLandingArea true if the collision is on the landing area
	/// @return the index of the crashed in line 
	int collisionWithSurface(
		const Coords& point0,
		const Coords& point1,
		bool& crashedInLandingArea
	) const;

	void addLine(
		const Coords& point0,
		const Coords& point1,
		int lineIdx,
		int landingZoneDirection,
		bool& landingZoneFound
	);

	float findDistanceToLandingZone(const Coords& from, int crashLineIdx) const;

#ifdef SVG
	string constructSVGData(const SVGManager& svgManager) const;
#endif // SVG

private:
	Line lines[MAX_LINES]; /// Native C++ array to hold the level linees
	int linesCount; /// Lines in current level
	int landingAreaLineIdx; /// Index of the landing area line
};

//*************************************************************************************************************
//*************************************************************************************************************

Surface::Surface() :
	linesCount(0),
	landingAreaLineIdx(INVALID_ID)
{

}

//*************************************************************************************************************
//*************************************************************************************************************

Surface::~Surface() {

}

//*************************************************************************************************************
//*************************************************************************************************************

int Surface::getLinesCount() const {
	return linesCount;
}

//*************************************************************************************************************
//*************************************************************************************************************

int Surface::collisionWithSurface(
	const Coords& point0,
	const Coords& point1,
	bool& crashedInLandingArea
) const {
	int crashLineIdx = INVALID_ID;

	for (size_t lineIdx = 0; lineIdx < linesCount; ++lineIdx) {
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
			//collisionPoint.setXCoord(p0x + (t * s1x));
			//collisionPoint.setYCoord(p0y + (t * s1y));

			crashLineIdx = static_cast<int>(lineIdx);
		}
	}

	crashedInLandingArea = landingAreaLineIdx == crashLineIdx;
	return crashLineIdx;
}

//*************************************************************************************************************
//*************************************************************************************************************

void Surface::addLine(
	const Coords& point0,
	const Coords& point1,
	int lineIdx,
	int landingZoneDirection,
	bool& landingZoneFound
) {
	Line line(point0, point1, landingZoneDirection);

	if (point0.getYCoord() == point1.getYCoord()) {
		landingZoneFound = true;
		landingAreaLineIdx = lineIdx;
		line.setLandingZoneDirection(LZD_HERE);
	}

	lines[lineIdx] = line;
}

//*************************************************************************************************************
//*************************************************************************************************************

#ifdef SVG
string Surface::constructSVGData(const SVGManager& svgManager) const {
	string svgStr = POLYLINE_BEGIN;

	for (size_t lineIdx = 0; lineIdx < linesCount; ++lineIdx) {
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

		// This check could be removed, because I think it is sure that the crash is not on landing zone
		if (crashedLine.getLandingZoneDirection() != LZD_HERE) {
			Coords pointToLandingZone = crashedLine.getPoint0();
			if (LZD_RIGHT == crashedLine.getLandingZoneDirection()) {
				pointToLandingZone = crashedLine.getPoint1();
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

	void setPosition(Coords position) { this->position = position; }
	void setHSpeed(float hSpeed) { this->hSpeed = hSpeed; }
	void setVSpeed(float vSpeed) { this->vSpeed = vSpeed; }
	void setFuel(int fuel) { this->fuel = fuel; }
	void setRotate(int rotate) { this->rotate = rotate; }
	void setPower(int power) { this->power = power; }

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
	void getTitleLines(vector<string>& titleLines) const;

private:
	Coords position;
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
	// It may be outside of the range, after a crossover
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

	const float newX = (float)position.getXCoord() + displacementX;
	const float newY = (float)position.getYCoord() + displacementY;

	const float newHSpeed = (float)hSpeed + accelerationX;
	const float newVSpeed = (float)vSpeed + accelerationY;

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

class Gene {
public:
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

	unsigned int getFlags() const {
		return flags;
	}

	void setShuttle(const Shuttle& shuttle) { this->shuttle = shuttle; }
	void setEvaluation(float evaluation) { this->evaluation = evaluation; }
	void setFlags(unsigned int flags) { this->flags = flags; }

	/// Reserve memory needed for the gene array of the Chromosome
	/// Set the initial state of the shuttle
	void init(const Shuttle& shuttle);

	/// Main thing to reset is the shuttle used for simulation to the initial state
	/// Other think as genes will be overwritten as they are calculated
	void reset();

	/// Divide the evaluation by the sum if all evaluations of all chromosomes in order to normalize it
	void normalizeEvaluation(float evaluationSum);

	void setFlag(int flag);
	void unsetFlag(int flag);
	bool hasFlag(int flag) const;
	void resetFlags();
	void addCrashLineIdxToFlags(int crashedLineIdx);
	int getCrashedLineIdx() const;

	bool operator<(const Chromosome& chromosome) const;
	Chromosome& operator=(const Chromosome& other);

	float evaluate(const Surface& surface);
	void insertGene(int geneIdx, const Gene& gene);
	const Gene& getGene(int geneIdx) const;

	/// Simulate the flight of the shuttle if solution is found return the needed information
	/// @param[in] surface the landing surface
	/// @param[out] goodForLanding bool if good for landing
	/// @param[out] lastGene the last gene, form the solution, which should be ignored
	void simulate(const Surface& surface, bool& goodForLanding, int& lastGene);

	void mutate();
	bool isValid();

	bool checkIfGoodForLanding(const Shuttle& previousShuttle, const Shuttle& shuttle) const;

#ifdef SVG
	Path getPath() const {
		return path;
	}

	float getOriginalEvaluation() const {
		return originalEvaluation;
	}

	void setPath(const Path& path) { this->path = path; }
	void setOriginalEvaluation(float originalEvaluation) { this->originalEvaluation = originalEvaluation; }

	string constructSVGData(const SVGManager& svgManager) const;
#endif // SVG

private:
	Shuttle initialShuttle; /// Shuttle form the beginnging of the turn
	Shuttle shuttle; /// Shuttle used for simulation

	float evaluation; /// Maybe I could work with integer evaluation !? experiment

	Gene chromosome[CHROMOSOME_SIZE];

	unsigned int flags; /// Stored chromosome properties

#ifdef SVG
	float originalEvaluation; /// For debug purposes
	Path path; /// Used to store the path which will be visualized in SVG, could be OPTIMIZED when solution is found
#endif // SVG
};

//*************************************************************************************************************
//*************************************************************************************************************

Chromosome::Chromosome() :
	shuttle(),
	evaluation(0.f),
	chromosome(),
	flags(0)
#ifdef SVG
	,
	originalEvaluation(0.f),
	path()
#endif // SVG
{
}

//*************************************************************************************************************
//*************************************************************************************************************

Chromosome::~Chromosome() {

#ifdef SVG
	path.clear();
#endif // SVG
}

//*************************************************************************************************************
//*************************************************************************************************************

void Chromosome::init(const Shuttle& shuttle) {
	initialShuttle = shuttle;
	this->shuttle = initialShuttle; // May be not nice code
}

//*************************************************************************************************************
//*************************************************************************************************************

void Chromosome::reset() {
	resetFlags();
	shuttle = initialShuttle;
}

//*************************************************************************************************************
//*************************************************************************************************************

void Chromosome::normalizeEvaluation(float evaluationSum) {
	evaluation /= evaluationSum;
}

void Chromosome::setFlag(int flag) {
	flags |= flag;
}

//*************************************************************************************************************
//*************************************************************************************************************

void Chromosome::unsetFlag(int flag) {
	flags &= ~flag;
}

//*************************************************************************************************************
//*************************************************************************************************************

bool Chromosome::hasFlag(int flag) const {
	return flags & flag;
}

//*************************************************************************************************************
//*************************************************************************************************************

void Chromosome::resetFlags() {
	flags = 0;
}

//*************************************************************************************************************
//*************************************************************************************************************

void Chromosome::addCrashLineIdxToFlags(int crashedLineIdx) {
	crashedLineIdx <<= CRASHED_IDX_MASK_OFFSET;
	flags |= crashedLineIdx;
}

//*************************************************************************************************************
//*************************************************************************************************************

int Chromosome::getCrashedLineIdx() const {
	unsigned int crashedLIneIdx = flags & CRASHED_IDX_MASK;
	crashedLIneIdx >>= CRASHED_IDX_MASK_OFFSET;

	return static_cast<int>(crashedLIneIdx);
}

//*************************************************************************************************************
//*************************************************************************************************************

bool Chromosome::operator<(const Chromosome& chromosome) const {
	return evaluation < chromosome.evaluation;
}

//*************************************************************************************************************
//*************************************************************************************************************

Chromosome& Chromosome::operator=(const Chromosome& other) {
//		if (this != &other) {
//			chromosome.shrink_to_fit();
//	
//			shuttle = other.shuttle;
//			evaluation = other.evaluation;
//			chromosome = other.chromosome;
//			flags = other.flags;
//	
//	#ifdef SVG
//			path.clear();
//			originalEvaluation = other.originalEvaluation;
//			path = other.path;
//	#endif // SVG
//		}
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
	evaluationLine.append(to_string(originalEvaluation));
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

	if (hasFlag(SELECTED_FLAG)) {
		strokeRGB = svgManager.constructStrokeForRGB(0, 0, 255);
	}
	else if (hasFlag(SOLUTION_FLAG)) {
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
#endif // SVG

//*************************************************************************************************************
//*************************************************************************************************************

float Chromosome::evaluate(const Surface& surface) {
	const Coord xPos = shuttle.getPosition().getXCoord();
	const Coord yPos = shuttle.getPosition().getYCoord();
	const bool validXPos = xPos >= 0 && xPos < MAP_WIDTH;
	const bool validYPos = yPos >= 0 && yPos < MAP_HEIGHT;
	
	if (!validXPos || !validYPos) {
		evaluation = 0.f;

#ifdef SVG
		originalEvaluation = 0.f;
#endif // SVG

		return evaluation;
	}

	const float hSpeed = shuttle.getHSpeed();
	const float vSpeed = shuttle.getVSpeed();
	const float currentSpeed = sqrt((hSpeed * hSpeed) + (vSpeed * vSpeed));
	const int rotation = shuttle.getRotate();

	if (!hasFlag(CRASHED_ON_LANDING_ZONE_FLAG)) {
		// 0-100: crashed somewhere, calculate score by distance to landing area
		const float distance = surface.findDistanceToLandingZone(shuttle.getPosition(), getCrashedLineIdx());

		// Calculate score from distance
		evaluation = 100.f - (100.f * distance / MAX_DISTANCE);

		// High speeds are bad, they decrease maneuvrability
		const float speedPen = 0.1f * max(currentSpeed - 100.f, 0.f);
		evaluation -= speedPen;
	}
	else if (vSpeed < -MAX_V_ABS_SPEED || MAX_H_ABS_SPEED < abs(hSpeed) || abs(rotation) > MAX_ROTATION_ANGLE_STEP) {
		// 100-200: crashed into landing area, calculate score by speed above safety
	
		float xPen = 0.f;
		if (MAX_H_ABS_SPEED < abs(hSpeed)) {
			xPen = (abs(hSpeed) - MAX_H_ABS_SPEED) / 2.f;
		}

		float yPen = 0.f;
		if (vSpeed < -MAX_V_ABS_SPEED) {
			yPen = (-MAX_V_ABS_SPEED - vSpeed) / 2.f;
		}

		//float rotationPen = 0.f;
		//if (abs(rotation) > MAX_ROTATION_ANGLE_STEP) {
		//	rotationPen = (abs(rotation) - MAX_ROTATION_ANGLE_STEP) / 2.f;
		//}

		evaluation = 200.f - xPen - yPen/* - rotationPen*/;
	}
	else {
		// 200-300: landed safely, calculate score by fuel remaining
		evaluation = 200.f + (100.f * shuttle.getFuel() / INITIAL_FUEL);
	}
#ifdef SVG
	originalEvaluation = evaluation;
#endif // SVG

	return evaluation;
}

//*************************************************************************************************************
//*************************************************************************************************************

void Chromosome::insertGene(int geneIdx, const Gene& gene) {
	chromosome[geneIdx] = gene; // Here copying is happening, which I'm not sure if is OK, have to measure, may be research moving semantic
}

//*************************************************************************************************************
//*************************************************************************************************************

const Gene& Chromosome::getGene(int geneIdx) const {
	return chromosome[geneIdx];
}

//*************************************************************************************************************
//*************************************************************************************************************

void Chromosome::simulate(const Surface& surface, bool& goodForLanding, int& lastGene) {
#ifdef SVG
	path.clear();
#endif // SVG

	// Use the last postion for shuttle to define a line and check if it crosses a surface line
	Shuttle previousShuttle;

	for (int geneIdx = 0; geneIdx < CHROMOSOME_SIZE; ++geneIdx) {
		const Gene& gene = chromosome[geneIdx];
		shuttle.simulate(gene.rotate, gene.power);

#ifdef SVG
		path.push_back(shuttle.getPosition());
#endif // SVG

		if (geneIdx > CHECK_FOR_CRASH_AFTER_GENE) {
			bool crashedInLandingArea = false;
			const int crashedLineIdx = surface.collisionWithSurface(previousShuttle.getPosition(), shuttle.getPosition(), crashedInLandingArea);

			if (INVALID_ID != crashedLineIdx) {
				setFlag(CRASHED_FLAG);
				addCrashLineIdxToFlags(crashedLineIdx);

				if (crashedInLandingArea) {
					setFlag(CRASHED_ON_LANDING_ZONE_FLAG);

					goodForLanding = checkIfGoodForLanding(previousShuttle, shuttle);
					if (goodForLanding) {
						// Ignore the gene which is after the crash
						lastGene = geneIdx;
#ifdef SVG
						for (size_t coordsIdx = 0; coordsIdx < path.size(); ++coordsIdx) {
							cout << '(' << path[coordsIdx].getXCoord() << ',' << path[coordsIdx].getYCoord() << "),";
							if (0 == coordsIdx % 10) { cout << endl; }
						}
						cout << endl;
#endif // SVG
					}
				}

				break;
			}
		}

		previousShuttle = shuttle; // I really want to not use copying, may be in favour of memory
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
	// return chromosome.size() > 0;
	return true;
}

//*************************************************************************************************************
//*************************************************************************************************************

bool Chromosome::checkIfGoodForLanding(const Shuttle& shuttleBeforeCrash, const Shuttle& shuttleAfterCrash) const {
	// If this function is called that means the crash is landing are

	const bool hSpeedBeforeCrashGood = abs(shuttleBeforeCrash.getHSpeed()) < MAX_H_ABS_SPEED;
	const bool vSpeedBeforeCrashGood = abs(shuttleBeforeCrash.getVSpeed()) < MAX_V_ABS_SPEED;
	const bool rotateBeforeCrashGood = abs(shuttleBeforeCrash.getRotate()) < MAX_ROTATION_ANGLE_STEP;

	// After the crash only the speed is important 
	const bool hSpeedAfterCrashGood = abs(shuttleAfterCrash.getHSpeed()) < MAX_H_ABS_SPEED;
	const bool vSpeedAfterCrashGood = abs(shuttleAfterCrash.getVSpeed()) < MAX_V_ABS_SPEED;

	return hSpeedBeforeCrashGood && vSpeedBeforeCrashGood && rotateBeforeCrashGood && hSpeedAfterCrashGood && vSpeedAfterCrashGood;
}

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

typedef map<float, int> ChromEvalIdxMap;

class GeneticPopulation {
public:
	GeneticPopulation();
	~GeneticPopulation();

	Surface getSurface() const {
		return surface;
	}

	int getPopulationId() const {
		return populationId;
	}

	const Chromosome& getChromosomeRef(int chromIdx) const {
		return population[chromIdx];
	}

	void setSurface(const Surface& surface) { this->surface = surface; }

	/// Reserve memory for 2 populations
	/// All chromosomes for all populations will use one indentical initial shuttle and modified it when simulating it
	/// so the shuttle will be set only once for all chromosomes and only will be reset when done with somilation
	/// @param[in] the initial shuttle for the game
	void init(const Shuttle& shuttle);

	/// Inserts directly in the active population with [] operator, no pushin or emplacing
	/// @param[in] the chromosome index where to insert
	/// @param[in] the chromosome's gene index which will be overwritten
	/// @paeam[in] gene the gene to insert
	void insertGene(int chromIdx, int geneIdx, const Gene& gene);

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
	/// @param[in] childrenCount how many children are already created
	void crossover(int parent0Idx, int parent1Idx, int childrenCount);

	/// Mutate the last two chromosomes in the children array, they are the new from the crossover
	/// @param[in] childrenCount how many children are already created
	void mutate(int childrenCount);

	/// Get the best chromosomes from the current population and pass them unchanged to the next
	void elitsm();

	/// Use the Continuos Genetic Algorithm methods to make the children for the new generation
	void makeChildren();

	/// Simulate all Chromosomes, using the commands from each gene to move the given shuttle
	/// @param[out] solutionChromIdx the solution chromosome index
	/// @param[out] lastGene the last gene, from the solution, which shloud be ignored
	bool simulate(int& solutionChromIdx, int& lastGene);

	/// Prepare the population for the roullete wheel selection:
	///		- calc the sum of evaluations
	///		- normalize the evalutions
	///		- sort the population's chromosome based on the cumulative sum
	///		- calc the cumulative sum
	void prepareForRoulleteWheel();

	/// Reset stats for each induvidual to the default values
	void reset();

	/// Plain copy chromosome: TODO: may be optimized
	/// @param[in] destIdx the chromosome in the new population
	/// @param[in] sourceIdx the chromosome in the old population
	void copyChromosomeToNewPopulation(int destIdx, int sourceIdx);

	/// Prepare for next turn
	void turnEnd();

#ifdef SVG
	/// Print the information for the current generation for visual debugging
	/// @param[in/out] svgManager the manager resposible for the visual debugging
	void visualDebugGeneration(SVGManager& svgManager) const;

	/// Fill the visula debug information
	string constructSVGData(const SVGManager& svgManager) const;
#endif // SVG

private:
	/// Will change the content in A when B is active and vise versa
	Chromosome populationA[POPULATION_SIZE];
	Chromosome populationB[POPULATION_SIZE];

	/// Points to active population
	Chromosome* population;

	/// Points to newly created population
	Chromosome* newPopulation;

	/// Holds chromosomes' evaluations as keys and chromosomes' indecies as values
	/// using map to hold this information, because every time a key is inserted it is stored in place (sorted)
	/// the map is represented as tree and every time an element is inserted I think is faster and sorting whole array of chromosomes
	/// TODO: measure the speed and if needed implement own hash map, no easy way to reserve map elements in advance,
	/// but I think map of floats and ints shouldn't be the bottle neck of the program
	ChromEvalIdxMap chromEvalIdxPairs;

	Surface surface;

	int populationId; /// For visual debug purposes
	float evaluationSum; /// Sum of all evaluations
};

//*************************************************************************************************************
//*************************************************************************************************************

GeneticPopulation::GeneticPopulation() :
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
	const int maxAngleRand = (2 * MAX_ROTATION_ANGLE_STEP) + 1;
	const int maxPowerRand = (2 * MAX_POWER_STEP) + 1;

	for (int chromIdx = 0; chromIdx < POPULATION_SIZE; ++chromIdx) {
		for (int geneIdx = 0; geneIdx < CHROMOSOME_SIZE; ++geneIdx) {
			int randAngle = rand() % maxAngleRand;
			int randPower = rand() % maxPowerRand;

			randAngle += MIN_ROTATION_ANGLE_STEP;
			randPower += MIN_POWER_STEP;

			const Gene gene(randAngle, randPower);
			insertGene(chromIdx, geneIdx, gene);
		}
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

bool GeneticPopulation::simulate(int& solutionChromIdx, int& lastGene) {
	bool foundResChromosome = false;

	for (int chromIdx = 0; chromIdx < POPULATION_SIZE; ++chromIdx) {
		Chromosome& chromosome = population[chromIdx];

		if (chromosome.hasFlag(COPIED_FLAG)) {
			// Directly transfered parent
			evaluationSum += chromosome.evaluate(surface); // Reset evaluation, because it was modified to fit the roullete wheel
			chromosome.unsetFlag(COPIED_FLAG); // Do not consider chromosome copied anymore
			continue;
		}

		chromosome.simulate(surface, foundResChromosome, lastGene);

		if (foundResChromosome) {
			solutionChromIdx = chromIdx;
			break;
		}

		evaluationSum += chromosome.evaluate(surface);
	}

	return foundResChromosome;
}

//*************************************************************************************************************
//*************************************************************************************************************

void GeneticPopulation::selectParentsIdxs(int& parent0Idx, int& parent1Idx) {
#ifdef SVG
	if (2 == POPULATION_SIZE) {
		parent0Idx = 0;
		parent1Idx = 1;

		return;
	}
#endif // SVG
	float r = Math::randomFloatBetween0and1();

	float cumulativeSum = 0.f;
	parent0Idx = chromEvalIdxPairs.rbegin()->second; // If r is too big the loop will break before setting the parentIdx
	for (ChromEvalIdxMap::const_iterator it = chromEvalIdxPairs.begin(); it != chromEvalIdxPairs.end(); ++it) {
		cumulativeSum += it->first;
		parent0Idx = it->second;

		if (r < cumulativeSum) {
			break;
		}
	}

	parent1Idx = parent0Idx;
	while (parent1Idx == parent0Idx) {
		r = Math::randomFloatBetween0and1();

		cumulativeSum = 0.f;
		parent1Idx = chromEvalIdxPairs.rbegin()->second; // If r is too big the loop will break before setting the parentIdx
		// Code duplication, at the moment I cannot think of more elegant layout
		for (ChromEvalIdxMap::const_iterator it = chromEvalIdxPairs.begin(); it != chromEvalIdxPairs.end(); ++it) {
			cumulativeSum += it->first;
			parent1Idx = it->second;

			if (r < cumulativeSum) {
				break;
			}
		}
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void GeneticPopulation::crossover(int parent0Idx, int parent1Idx, int childrenCount) {
	const float crossoverRand0 = Math::randomFloatBetween0and1();
	const float crossoverRand1 = Math::randomFloatBetween0and1();

	//bool useParent0 = false;
	//if (crossoverRand0 > PROBABILITY_OF_CROSSOVER) {
	//	copyChromosomeToNewPopulation(childrenCount, parent0Idx);
	//	useParent0 = true;
	//}
	//
	//bool useParent1 = false;
	//if (crossoverRand1 > PROBABILITY_OF_CROSSOVER) {
	//	copyChromosomeToNewPopulation(childrenCount + 1, parent1Idx);
	//	useParent1 = true;
	//}
	//
	//if (useParent0 && useParent1) {
	//	return;
	//}

	const Chromosome& parent0 = population[parent0Idx];
	const Chromosome& parent1 = population[parent1Idx];

	for (int geneIdx = 0; geneIdx < CHROMOSOME_SIZE; ++geneIdx) {
		const Gene& parent0Gene = parent0.getGene(geneIdx);
		const Gene& parent1Gene = parent1.getGene(geneIdx);

		const float beta = Math::randomFloatBetween0and1();
		const float parent0Rotate = static_cast<float>(parent0Gene.rotate);
		const float parent1Rotate = static_cast<float>(parent1Gene.rotate);
		const float parent0Power = static_cast<float>(parent0Gene.power);
		const float parent1Power = static_cast<float>(parent1Gene.power);

		const float child0Rotation = (beta * parent0Rotate) + ((1.f - beta) * parent1Rotate);
		const float child1Rotation = ((1.f - beta) * parent0Rotate) + (beta * parent1Rotate);
		const float child0Power = (beta * parent0Power) + ((1.f - beta) * parent1Power);
		const float child1Power = ((1.f - beta) * parent0Power) + (beta * parent1Power);

		Gene child0Gene;
		child0Gene.rotate = static_cast<int>(round(child0Rotation));
		child0Gene.power = static_cast<int>(round(child0Power));

		Gene child1Gene;
		child1Gene.rotate = static_cast<int>(round(child1Rotation));
		child1Gene.power = static_cast<int>(round(child1Power));

		//if (crossoverRand0 <= PROBABILITY_OF_CROSSOVER) {
			newPopulation[childrenCount].insertGene(geneIdx, child0Gene);
		//}

		//if (crossoverRand1 <= PROBABILITY_OF_CROSSOVER) {
			newPopulation[childrenCount + 1].insertGene(geneIdx, child1Gene);
		//}
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void GeneticPopulation::mutate(int childrenCount) {
	// Last two chromosomes are made during the crossover
	newPopulation[childrenCount].mutate();
	newPopulation[childrenCount + 1].mutate();
}

//*************************************************************************************************************
//*************************************************************************************************************

void GeneticPopulation::elitsm() {
	const int elitsCount = static_cast<int>(POPULATION_SIZE * ELITISM_RATIO);

	// Use the map, which holds sorted evaluations as keys
	ChromEvalIdxMap::const_reverse_iterator it = chromEvalIdxPairs.rbegin();
	for (int elitIdx = 0; elitIdx < elitsCount; ++elitIdx) {
		const int chromosomeIdx = it->second;

		copyChromosomeToNewPopulation(elitIdx, chromosomeIdx);
		++it;
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void GeneticPopulation::makeChildren() {
	// While the new population is not completly filled
	// select a pair of parents
	// crossover those parents using the Continuos Genetic Algorithm technique
	// mutate them using the Continuos Genetic Algorithm technique
	for (int childrenCount = 0; childrenCount < POPULATION_SIZE; childrenCount += 2) {
		int parent0Idx = INVALID_ID; // For safety reasons
		int parent1Idx = INVALID_ID; // For safety reasons

		selectParentsIdxs(parent0Idx, parent1Idx);

#ifdef SVG
		population[parent0Idx].setFlag(SELECTED_FLAG);
		population[parent1Idx].setFlag(SELECTED_FLAG);
#endif // SVG

		crossover(parent0Idx, parent1Idx, childrenCount);
		mutate(childrenCount);
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void GeneticPopulation::init(const Shuttle& shuttle) {
	for (int chromIdx = 0; chromIdx < POPULATION_SIZE; ++chromIdx) {
		populationA[chromIdx].init(shuttle);
		populationB[chromIdx].init(shuttle);
	}

	// A strats as active population, for it random population will be made
	population = populationA;
	newPopulation = populationB;
}

//*************************************************************************************************************
//*************************************************************************************************************

void GeneticPopulation::insertGene(int chromIdx, int geneIdx, const Gene& gene) {
	population[chromIdx].insertGene(geneIdx, gene);
}

//*************************************************************************************************************
//*************************************************************************************************************

void GeneticPopulation::makeNextGeneration(
#ifdef SVG
	SVGManager& svgManager
#endif // SVG
) {
	prepareForRoulleteWheel();
	makeChildren();

	// Apply elitism, get the best chromosomes from the population and overwrite some children
	elitsm();

#ifdef SVG
	visualDebugGeneration(svgManager);
#endif // SVG

	reset();
	++populationId;
}


//*************************************************************************************************************
//*************************************************************************************************************

void GeneticPopulation::prepareForRoulleteWheel() {
	for (int chromIdx = 0; chromIdx < POPULATION_SIZE; ++chromIdx) {
		Chromosome& chromosome = population[chromIdx];
		const float normalizedEvaluation = chromosome.getEvaluation() / evaluationSum; // normalize the evalutions
		chromosome.setEvaluation(normalizedEvaluation);

		chromEvalIdxPairs[normalizedEvaluation] = chromIdx; // Is it good think to use floats as keys
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void GeneticPopulation::reset() {
	evaluationSum = 0.f;
	chromEvalIdxPairs.clear();

	// If copied directly from previous population do not reset
	for (int chromIdx = 0; chromIdx < POPULATION_SIZE; ++chromIdx) {
		Chromosome& chromosome = newPopulation[chromIdx];
		if (!chromosome.hasFlag(COPIED_FLAG)) {
			chromosome.reset();
		}
	}

	// Switch population arrays
	if (0 == (populationId % 2)) {
		population = populationB;
		newPopulation = populationA;
	}
	else {
		population = populationA;
		newPopulation = populationB;
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void GeneticPopulation::copyChromosomeToNewPopulation(int destIdx, int sourceIdx) {
	const Chromosome& sourceChromosome = population[sourceIdx];
	Chromosome& destinationChromosome = newPopulation[destIdx];

	destinationChromosome.setEvaluation(sourceChromosome.getEvaluation());
	destinationChromosome.setFlags(sourceChromosome.getFlags());
	destinationChromosome.setShuttle(sourceChromosome.getShuttle());
	for (int geneIdx = 0; geneIdx < CHROMOSOME_SIZE; ++geneIdx) {
		destinationChromosome.insertGene(geneIdx, sourceChromosome.getGene(geneIdx));
	}

	// Set at the end of copying to not be overwritten
	destinationChromosome.setFlag(COPIED_FLAG);

#ifdef SVG
	destinationChromosome.setPath(sourceChromosome.getPath());
	destinationChromosome.setOriginalEvaluation(sourceChromosome.getOriginalEvaluation());
#endif // SVG
}

//*************************************************************************************************************
//*************************************************************************************************************

void GeneticPopulation::turnEnd() {

	populationId = 0;
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

	for (int chromeIdx = 0; chromeIdx < POPULATION_SIZE; ++chromeIdx) {
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

	void initGame();
	void gameBegin();
	void gameLoop();
	void getGameInput();
	void getTurnInput();
	void turnBegin();
	void makeTurn(bool& notDone);
	void turnEnd();
	void play();
	void gameEnd();

	void debug() const;

private:
	int lastPower; /// The power from previous turn
	int turnsCount;

	Shuttle shuttle;
	Surface surface;

	GeneticPopulation geneticPopulation;

	/// The solution of the GA
	int solutionChromIdx;

	/// The last gene, form the solution, which should be ignored
	int lastGene;

	/// Which gene to output from the chromosome solution
	int turnGeneIdx;

#ifdef SVG
	SVGManager svgManager;
#endif // SVG

};

//*************************************************************************************************************
//*************************************************************************************************************

Game::Game() :
	lastPower(0),
	turnsCount(0),
	shuttle(),
	surface(),
	geneticPopulation(),
	solutionChromIdx(INVALID_ID),
	lastGene(INVALID_ID),
	turnGeneIdx(0)
#ifdef SVG
	,svgManager()
#endif // SVG
{
}

//*************************************************************************************************************
//*************************************************************************************************************

Game::~Game() {
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::initGame() {
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::gameBegin() {
	geneticPopulation.setSurface(surface);

#ifdef SVG
	string surfaceSVGData = surface.constructSVGData(svgManager);
	svgManager.filePrintStr(surfaceSVGData);
#endif // SVG
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::gameLoop() {
	bool notDone = true;

	while (notDone) {
		getTurnInput();
		turnBegin();
		makeTurn(notDone);
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
	surface.setLinesCount(surfaceN - 1); // Minus 1, because this is the count of points forming the lines

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

			// Line index is - 1, because the second point is ith index
			surface.addLine(point0, point1, i - 1, landingZoneDirection, landingZoneFound);
		}

		point0 = point1;
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

	// Get data for the shuttle if solution is not found, because everything from the genetic algorithm will be applied on top of the initial shuttle
	// And there may be differences from the online platform if I use each new position
	if (INVALID_ID == solutionChromIdx) {
		shuttle.setPosition(Coords(static_cast<float>(X), static_cast<float>(Y)));
		shuttle.setHSpeed(static_cast<float>(hSpeed));
		shuttle.setVSpeed(static_cast<float>(vSpeed));
		shuttle.setFuel(fuel);
		shuttle.setRotate(rotate);
		shuttle.setPower(power);
	}

	if (0 == turnsCount) {
		INITIAL_FUEL = fuel;
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::turnBegin() {
	// Run the genetic algorithm every turn if no solution is found
	if (INVALID_ID != solutionChromIdx) {
		return;
	}

	geneticPopulation.init(shuttle);
	geneticPopulation.initRandomPopulation();

	bool answerFound = false;
	while (!answerFound && geneticPopulation.getPopulationId() <= MAX_POPULATION) {
		answerFound = geneticPopulation.simulate(solutionChromIdx, lastGene);

		if (answerFound) {
#ifdef SVG
			geneticPopulation.visualDebugGeneration(svgManager);
#endif // SVG

			break;
		}

		geneticPopulation.makeNextGeneration(
#ifdef SVG
			svgManager
#endif // SVG
		);
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::makeTurn(bool& notDone) {
#ifdef SVG
	if (INVALID_ID != solutionChromIdx) {
		notDone = false;
		return;
	}
#endif // SVG

	int solutionIdx = solutionChromIdx;
	if (INVALID_ID == solutionChromIdx) {
		solutionChromIdx = 0; // Override the solution value, if no time to simulate in each turn
		solutionIdx = 0;
	}

	char separator = ' ';
#ifdef SIMULATION_OUTPUT
	separator = ',';
#endif // SIMULATION_OUTPUT

	if (turnGeneIdx < lastGene) {
		const Chromosome& solutionChromosome = geneticPopulation.getChromosomeRef(solutionIdx);
		shuttle.applyNewRotateAngle(solutionChromosome.getGene(turnGeneIdx).rotate);
		shuttle.applyNewPower(solutionChromosome.getGene(turnGeneIdx).power);

		lastPower = shuttle.getPower();
		cout << shuttle.getRotate() << separator << lastPower << endl;
	}
	else if (turnGeneIdx == lastGene) {
		cout << 0 << separator << lastPower << endl;
	}
	else {
		cout << 0 << separator << 0 << endl;
	}

	if (INVALID_ID != solutionChromIdx) {
		++turnGeneIdx;
	}

#ifdef SIMULATION_OUTPUT
	cout << ',';
#endif // SIMULATION_OUTPUT

	if (turnsCount > lastGene + ADDITIONAL_TURNS) {
		notDone = false;
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void Game::turnEnd() {
	++turnsCount;

	if (INVALID_ID == solutionChromIdx) {
		geneticPopulation.turnEnd();
	}
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