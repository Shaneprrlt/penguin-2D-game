#include "grafix.h"
#include "AnimationWrapper.h"

/*
 * Written by Shane Perreault 11/8/2014
 */

/*
 * Entry point for game, initializes the
 * animation wrapper which houses all logic
 * for the game.
 */
int main()
{
	// initiate the game!	
	AnimationWrapper::AnimationWrapper(PerX(60), PerY(80)).onCreate();

	return 0;
}