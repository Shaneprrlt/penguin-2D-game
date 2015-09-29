#pragma once
#include "grafix.h"

/*
 * Bullets are what are fired
 * from not just the player,
 * but also the enemies.
 * Written by Shane Perreault 11/9/2014
 */

class Bullet {
public:

	// member data about the bullet
	Point pos;
	Color color;
	int size;
	int xVel;

	// default constructor
	Bullet();

	// constructor
	Bullet(Point pos, Color color, int size, int xVel);

	// destructor
	~Bullet();

	// draws the bullet
	void drawBullet();
};
