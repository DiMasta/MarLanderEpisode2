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
//#define SIMULATION_OUTPUT

#ifdef SVG
#include "SVGManager.h"
#endif // SVG

using namespace std;

const bool OUTPUT_GAME_DATA = 0;

const int MAP_WIDTH = 7000;
const int MAP_HEIGHT = 3000;
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

const string INPUT_FILE_NAME = "input.txt";
const string OUTPUT_FILE_NAME = "output.txt";

const float MARS_GRAVITY = 3.711f;
const int CHROMOSOME_SIZE = 60;
const int POPULATION_SIZE = 40;
const int BEST_CHROMOSOMES_COUNT = static_cast<int>(POPULATION_SIZE * BEST_CHROMOSOMES_PERCENT);
const int OTHERS_CHROMOSOMES_COUNT = static_cast<int>(POPULATION_SIZE * OTHERS_CHROMOSOMES_PERCENT);
const int CHILDREN_COUNT = 4;
const int CROSSOVER_POINT = POPULATION_SIZE / 2;
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

	int getLinesCount() const;
	Line getLine(int lineIdx) const;
	bool collisionWithSurface(const Coords& landerPoint);
	void addLine(
		const Coords& point0,
		const Coords& point1,
		int landingZoneDirection,
		bool& landingZoneFound
	);

	float findDistanceToLandingZone(const Coords& from) const;

	string constructSVGData(const SVGManager& svgManager) const;

private:
	Lines lines;
	Line landingZone;
};

//*************************************************************************************************************
//*************************************************************************************************************

Surface::Surface() :
	lines(),
	landingZone()
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

float Surface::findDistanceToLandingZone(const Coords& from) const {
	size_t nearestLineIdx = 0;
	int nearestLineDirToLandingZone = LZD_INVALID;

	Coord minDistToLine = FLT_MAX;
	for (size_t lineIdx = 0; lineIdx < lines.size(); ++lineIdx) {
		const Line* line = &lines[lineIdx];

		Coord linePointDistance = line->distanceToPoint(from);

		if (linePointDistance < minDistToLine) {
			minDistToLine = linePointDistance;
			nearestLineIdx = lineIdx;
			nearestLineDirToLandingZone = line->getLandingZoneDirection();
		}
	}

	const Line* nearestLine = &lines[nearestLineIdx];
	Coords pointToLandingZone = nearestLine->getPoint0();
	if (LZD_RIGHT == nearestLineDirToLandingZone) {
		pointToLandingZone = nearestLine->getPoint1();
	}
	else if (LZD_HERE == nearestLineDirToLandingZone) {
		Coord x0 = nearestLine->getPoint0().getXCoord();
		Coord y0 = nearestLine->getPoint0().getYCoord();
		Coord x1 = nearestLine->getPoint1().getXCoord();

		pointToLandingZone.setXCoord(x0 + ((x1 - x0) / 2));
		pointToLandingZone.setYCoord(y0);
	}

	float distTolandingZone = distance(from, pointToLandingZone);
	const Line* line = nearestLine;

	int lineIdx = nearestLineIdx;
	while (true) {
		if (LZD_RIGHT == line->getLandingZoneDirection()) {
			++lineIdx;
		}
		else if (LZD_LEFT) {
			--lineIdx;
		}

		line = &lines[lineIdx];
		int lineLenght = line->getLenght();
		
		if (LZD_HERE == line->getLandingZoneDirection()) {
			distTolandingZone += lineLenght / 2;
			break;
		}

		distTolandingZone += lineLenght;
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

	Coords getPreviousPosition() const {
		return previousPosition;
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

	int clampRotateAngle(int newRotateAngle) const;
	int clampPower(int newPower) const;

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

Shuttle::Shuttle(const Shuttle& shuttle) {
	position = shuttle.position;
	previousPosition = shuttle.previousPosition;
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
	cout << "Turn=" << turn++ << endl;
	cout << "X=" << newX << "m, Y=" << newY << "m, ";
	cout << "HSPeed=" << newHSpeed << "m/s VSpeed=" << newVSpeed << "m/s\n";
	cout << "Fuel=" << newFuel << "l, Angle=" << newAngle << ", Power=" << newPower << "m/s2\n";
	cout << endl << endl;
#endif // SIMULATION_OUTPUT
}

//*************************************************************************************************************
//*************************************************************************************************************

void Shuttle::print() const {
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

	Genes getChromosome() const {
		return chromosome;
	}

	Path getPath() const {
		return path;
	}

	void setShuttle(const Shuttle& shuttle) { this->shuttle = shuttle; }
	void setEvaluation(float evaluation) { this->evaluation = evaluation; }
	void setChromosome(const Genes& chromosome) { this->chromosome = chromosome; }
	void setPath(const Path& path) { this->path = path; }

	bool operator<(const Chromosome& chromosome) const;
	Chromosome& operator=(const Chromosome& other);

	void evaluate(Surface* surface);
	void addGene(const Gene& gene);
	Gene getGene(int geneIdx) const;
	void simulate(Surface* surface);
	void mutate();

	string constructSVGData(const SVGManager& svgManager) const;

private:
	Shuttle shuttle;
	float evaluation;

	Genes chromosome;
	Path path;
};

//*************************************************************************************************************
//*************************************************************************************************************

Chromosome::Chromosome() :
	shuttle(),
	evaluation(0.f),
	chromosome(),
	path()
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
	}
	return *this;
}

//*************************************************************************************************************
//*************************************************************************************************************

string Chromosome::constructSVGData(const SVGManager& svgManager) const {
	string svgStr = "";

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

	svgStr.append(DATA_EVALUATION);
	svgStr.append(to_string(evaluation));
	svgStr.append(ATTRIBUTE_END);

	svgStr.append(STYLE_BEGIN);
	svgStr.append(FILL_NONE);

	string strokeRGB = svgManager.constructStrokeForRGB(255, 0, 0);
	svgStr.append(strokeRGB);
	svgStr.append(";");

	string strokeWidth = svgManager.constructStrokeWidth(10);
	svgStr.append(strokeWidth);
	svgStr.append(ATTRIBUTE_END);

	svgStr.append(ONCLICK_DISPLAY_DATA);

	svgStr.append(POLYLINE_END);

	return svgStr;
}

//*************************************************************************************************************
//*************************************************************************************************************

void Chromosome::evaluate(Surface* surface) {
	evaluation = surface->findDistanceToLandingZone(shuttle.getPosition());
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

void Chromosome::simulate(Surface* surface) {
	for (int geneIdx = 0; geneIdx < CHROMOSOME_SIZE; ++geneIdx) {
		Gene* gene = &chromosome[geneIdx];
		shuttle.simulate(gene->rotate, gene->power);
		path.push_back(shuttle.getPosition());

		if (surface->collisionWithSurface(shuttle.getPosition())) {
			break;
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

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

typedef vector<Chromosome> Chromosomes;

class GeneticPopulation {
public:
	GeneticPopulation();
	~GeneticPopulation();

	Surface getSurface() const {
		return surface;
	}

	void setSurface(const Surface& surface) { this->surface = surface; }

	void initRandomPopulation();
	void simulate(Shuttle* shuttle);
	void sortChromosomes();
	void chooseParents(Chromosomes& parents);
	void makeChildren(Chromosomes& parents, Chromosomes& children);
	void crossover(const Chromosome& parent0, const Chromosome& parent1, Chromosome& child);
	void makeNextGeneration();

	string constructSVGData(const SVGManager& svgManager) const;

private:
	Chromosomes population;
	Surface surface;
};

//*************************************************************************************************************
//*************************************************************************************************************

GeneticPopulation::GeneticPopulation() :
	population(POPULATION_SIZE),
	surface()
{
}

//*************************************************************************************************************
//*************************************************************************************************************

GeneticPopulation::~GeneticPopulation() {
}

//*************************************************************************************************************
//*************************************************************************************************************

void GeneticPopulation::initRandomPopulation() {
	random_device angleRd; // obtain a random number from hardware
	mt19937 angleEng(angleRd()); // seed the generator
	uniform_int_distribution<> rotateDistr(MIN_ROTATION_ANGLE, MAX_ROTATION_ANGLE); // define the range

	random_device powerRd;
	mt19937 powerEng(powerRd());
	uniform_int_distribution<> powerDistr(MIN_POWER, MAX_POWER);

	for (int chromIdx = 0; chromIdx < POPULATION_SIZE; ++chromIdx) {
		for (int geneIdx = 0; geneIdx < CHROMOSOME_SIZE; ++geneIdx) {
			int randAngle = rotateDistr(angleEng);
			int randPower = powerDistr(powerEng);

			// Hardcoded oldschool rand
			//int randAngle = (rand() % 181) - 90;
			//int randPower = (rand() % 5);

			Gene gene(randAngle, randPower);
			population[chromIdx].addGene(gene);

#ifdef SIMULATION_OUTPUT
			cout << randAngle << ", " << randPower << ", " << endl;
#endif // SIMULATION_OUTPUT

		}
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void GeneticPopulation::simulate(Shuttle* shuttle) {
	for (size_t chromIdx = 0; chromIdx < population.size(); ++chromIdx) {
		population[chromIdx].setShuttle(*shuttle);
		population[chromIdx].simulate(&surface);
		population[chromIdx].evaluate(&surface);
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void GeneticPopulation::sortChromosomes() {
	sort(population.begin(), population.end());
}

//*************************************************************************************************************
//*************************************************************************************************************

void GeneticPopulation::chooseParents(Chromosomes& parents) {
	copy(population.begin(), population.begin() + BEST_CHROMOSOMES_COUNT, back_inserter(parents));

	Chromosomes othersChromosomes;
	copy(population.begin() + BEST_CHROMOSOMES_COUNT, population.end(), back_inserter(othersChromosomes));

	unsigned seed = static_cast<unsigned>(chrono::system_clock::now().time_since_epoch().count());
	shuffle(othersChromosomes.begin(), othersChromosomes.end(), default_random_engine(seed));

	othersChromosomes.erase(
		othersChromosomes.begin() + OTHERS_CHROMOSOMES_COUNT,
		othersChromosomes.end()
	);

	parents.insert(parents.end(), othersChromosomes.begin(), othersChromosomes.end());
}

//*************************************************************************************************************
//*************************************************************************************************************

void GeneticPopulation::makeChildren(Chromosomes& parents, Chromosomes& children) {
	unsigned seed = static_cast<unsigned>(chrono::system_clock::now().time_since_epoch().count());
	shuffle(parents.begin(), parents.end(), default_random_engine(seed));

	int chosenParentsCount = 2 * CHILDREN_COUNT;
	Chromosomes chosenParents(parents.begin(), parents.begin() + chosenParentsCount);

	for (int childIdx = 0; childIdx < CHILDREN_COUNT; ++childIdx) {
		Chromosome* parent0 = &chosenParents[childIdx * 2];
		Chromosome* parent1 = &chosenParents[(childIdx * 2) + 1];

		Chromosome child;
		crossover(*parent0, *parent1, child);
		child.mutate();
		children.push_back(child);
	}
}

//*************************************************************************************************************
//*************************************************************************************************************

void GeneticPopulation::crossover(const Chromosome& parent0, const Chromosome& parent1, Chromosome& child) {
	Genes childGenes;

	for (int geneIdx = 0; geneIdx < CHROMOSOME_SIZE; ++geneIdx) {
		Gene* gene = &parent0.getGene(geneIdx);

		if (geneIdx >= CROSSOVER_POINT) {
			gene = &parent1.getGene(geneIdx);
		}

		childGenes.push_back(*gene);
	}

	child.setChromosome(childGenes);
}

//*************************************************************************************************************
//*************************************************************************************************************

void GeneticPopulation::makeNextGeneration() {
	Chromosomes parents;
	chooseParents(parents);

	Chromosomes children;
	makeChildren(parents, children);

	population.clear();
	population = parents;
	population.insert(population.end(), children.begin(), children.end());
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
	geneticPopulation()
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

	bool landingZoneFound = false;
	for (int i = 0; i < surfaceN; i++) {
		int landX; // X coordinate of a surface point. (0 to 6999)
		int landY; // Y coordinate of a surface point. By linking all the points together in a sequential fashion, you form the surface of Mars.
		cin >> landX >> landY; cin.ignore();

		if (OUTPUT_GAME_DATA) {
			cerr << landX << " " << landY << endl;
		}

		int landingZoneDirection = LZD_RIGHT;
		if (landingZoneFound) {
			landingZoneDirection = LZD_LEFT;
		}

		Coords point1((float)landX, (float)landY);

		if (0 != i) {
			surface->addLine(point0, point1, landingZoneDirection, landingZoneFound);
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
		geneticPopulation.simulate(shuttle);
		geneticPopulation.sortChromosomes();
#ifdef SVG
		string populationSVGData = geneticPopulation.constructSVGData(svgManager);
		string populationIdSVGData = svgManager.constructGId(populationId);
		svgManager.filePrintStr(populationIdSVGData);
		svgManager.filePrintStr(populationSVGData);
		svgManager.filePrintStr(GROUP_END);
#endif // SVG

		geneticPopulation.makeNextGeneration();

		if (1 == populationId++) {
			break;
		}
	}
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
	// render the turn debug data

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