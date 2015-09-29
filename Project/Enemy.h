#pragma once
#include "grafix.h"
#include <vector>
#include "Bullet.h"

/*
 * Enemies are the antagonists
 * in the game and are trying
 * to approach the player and
 * destroy them with bullets
 * the user must jump to avoid.
 * Written by Shane Perreault 11/8/2014
 */

class Enemy {
public:

	// member data about enemy
	Point pos;
	int size;
	int xVel;
	
	// color info
	Color enemyColor;
	Color enemyFaceColor;

	// default constructor
	Enemy();

	// destructor
	~Enemy();

	// draws the enemy to the screen
	void drawEnemy();

	// kills the enemy
	void kill();
};
