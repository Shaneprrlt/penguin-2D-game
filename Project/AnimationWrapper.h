#pragma once
#include "Game.h"
#include "Map.h"
#include "Player.h"
#include "Enemy.h"

/*
 * AnimationWrapper contains all the game
 * pieces and organizes all the animation
 * into one place.
 * Written by Shane Perreault 11/8/2014
 */

/*
 * Defines the Animation Wrapper
 */
class AnimationWrapper {
public:

	// width of the window
	int WINDOW_WIDTH;

	// height of the window
	int WINDOW_HEIGHT;

	// the game state and general rules
	Game gHandler;

	// the level map
	Map map;

	// constructor
	AnimationWrapper(int width, int height);

	// get the left boundary
	int getLeftBoundary();

	// get the right boundary
	int getRightBoundary();

	// get the top boundary
	int getTopBoundary();

	// get the bottom boundary
	int getBottomBoundary();

	// draws the ribbon on top of the window
	void drawGameRibbon();

	// takes in user input and handles accordingly
	void update(int animationPercent);

	// initiates game loop
	void onCreate();

	// game controls
	void runGameControls();

	// draws the pause menu
	void drawPauseMenu();

};