#pragma once
#include "grafix.h"

/*
 * Map builds the general level
 * and draws the world the player
 * plays the game in.
 * Written by Shane Perreault 11/9/2014
 */

class Map {
public:

	// the boundaries of the window
	// so the level is not drawn off
	// the screen
	int windowBoundaryLeft;
	int windowBoundaryRight;
	int windowBoundaryTop;
	int windowBoundaryBottom;

	// default constructor
	Map();

	// constructor
	Map(int wBLeft, int wBRight, int wBTop, int wBBottom);

	// destructor
	~Map();

	// draws the floor
	void drawFloor();

	// returns the y-coordinate of the floor
	int getFloor();

};
