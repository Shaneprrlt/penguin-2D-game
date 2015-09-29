#include "Map.h"
#include "grafix.h"

// default constructor
Map::Map() {}

// constructor
Map::Map(int wBLeft, int wBRight, int wBTop, int wBBottom) {
	this->windowBoundaryLeft = wBLeft;
	this->windowBoundaryRight = wBRight;
	this->windowBoundaryTop = wBTop;
	this->windowBoundaryBottom = wBBottom;
}

// destructor
Map::~Map() {}

// draws the floor
void Map::drawFloor() {
	
	// draw a basic line
	draw_line(this->windowBoundaryLeft,
		this->windowBoundaryBottom - PerY(20), this->windowBoundaryRight,
			this->windowBoundaryBottom - PerY(20));
}

// gets the y-coordinate of the floor
int Map::getFloor() {
	// the math for the floor of the level
	return this->windowBoundaryBottom - PerY(20);
}