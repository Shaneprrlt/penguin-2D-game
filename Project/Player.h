#pragma once
#include "grafix.h"
#include <vector>
#include "Bullet.h"

/*
 * Player is the game character
 * that is controlled by the user
 * and is the protaganist.
 * Written by Shane Perreault 11/8/2014
 */

class Player {
public:

	// member data about the player
	Point pos;
	int size;
	bool facingEast;
	int xVel;
	int yVel;
	vector<Bullet> shots;

	// jumping information
	bool can_jump;
	bool falling;
	int jumpVelocity;
	int jumpBoundary;
	int gravity;

	// player default constructor
	Player();

	// player class constructor method
	Player(Point pos, int size, int xVel, int yVel = 10, int jVel = 8, int jB = 125);

	// draws the floor
	void drawBase();

	// draws the player
	void drawPlayer();

	// makes the player jump
	void jump(int returnY);

	// makes the player fire a shot
	void fire();

};
