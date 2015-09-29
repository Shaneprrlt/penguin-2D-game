#pragma once
#include <vector>
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"

/**
 * Game stores all scoring and character
 * information in the game. Initiates
 * certain callbacks when triggered by
 * specific events.
 * Written by Shane Perreault 11/8/2014
 */

class Game {
public:
	// constructor
	Game();
	
	// the players current score
	int PLAYER_SCORE;

	// the players current lives
	int PLAYER_LIVES;

	// the current player object
	Player player;

	// sets the player to the active game
	void setPlayer(Player p);
	
	// the current enemies
	std::vector<Enemy> enemies;

	// the shots fired at player
	std::vector<Bullet> enemyShots;

	// spawns a new enemy to the game state
	void spawnEnemy(int floor, int leftBoundary, int rightBoundary);

	// adds an enemy to an active game
	void addEnemy(Enemy e);

	// detects bullet collisions
	void detectHits(bool gameMuted);

};