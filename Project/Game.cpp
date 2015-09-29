#include "Game.h"
#include <vector>
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"

// prototype functions needed
bool inRange(int min, int max, int i);
bool intersects(Bullet bullet, Enemy enemy);
bool intersects(Bullet bullet, Player player);

/**
 * Game class function definitions.
 * Written by Shane Perreault 11/8/2014
 */

// game constructor
Game::Game() {}

// sets the current game player
void Game::setPlayer(Player p) {
	this->player = p;
}

// spanws a new enemy to the game state
void Game::spawnEnemy(int floor, int leftBoundary, int rightBoundary) {
	Enemy e;
	int flag = random(-1, 1);
	while(flag == 0)
		flag = random(-1, 1);
	e.size = 20;
	e.xVel = flag * random(1, 4);
	int xPos = flag > 0 ? leftBoundary + random(e.size, 100)
		: rightBoundary - random(e.size, 100);
	e.pos = Point::Point(xPos, floor - e.size);
	this->enemies.push_back(e);
}

// adds an enemy to current game
void Game::addEnemy(Enemy e) {
	this->enemies.push_back(e);
}

// detects bullet hits
void Game::detectHits(bool gameMuted) {
	// loop through each bullet and detect
	// collsion against each enemy
	for(int i = 0; i < this->player.shots.size(); i++) {
		for(int j = 0; j < this->enemies.size(); j++) {
			// if the two elements intersect
			if(intersects(this->player.shots[i], this->enemies[j])) {
				this->player.shots.erase(this->player.shots.begin() + i);
				this->enemies.erase(this->enemies.begin() + j);
				this->PLAYER_SCORE += 10;
				if(!gameMuted)
					play_sound("impact.wav");
				break;
			}
		}
	}

	// loop through enemy bullets and detect
	// collision against player
	for(int i = 0; i < this->enemyShots.size(); i++ ) {
		if(intersects(this->enemyShots[i], this->player)) {
			this->enemyShots.erase(this->enemyShots.begin() + i);
			this->PLAYER_LIVES--;
		}
	}
}

// helpful function for calculating
// whether i is in the closed range [min, max]
bool inRange(int min, int max, int i) {
	return (min <= i) && (i <= max);
}

// determines if a bullet and enemy intersect 
bool intersects(Bullet bullet, Enemy enemy) {
	bool xIntersects, yIntersects;
	xIntersects = inRange(enemy.pos.x - enemy.size / 2,
		enemy.pos.x + enemy.size / 2, bullet.pos.x + bullet.size);
	if(bullet.xVel < 0) {
		xIntersects = inRange(enemy.pos.x - enemy.size / 2,
			enemy.pos.x + enemy.size / 2, bullet.pos.x - bullet.size);
	}
	yIntersects = inRange(enemy.pos.y - enemy.size, 
		enemy.pos.y + enemy.size, bullet.pos.y - bullet.size / 2)
		|| inRange(enemy.pos.y - enemy.size,
		enemy.pos.y + enemy.size, bullet.pos.y + bullet.size / 2);
	return xIntersects && yIntersects;
}

// determines if a bullet and player intersect
bool intersects(Bullet bullet, Player player) {
	bool xIntersects, yIntersects;
	xIntersects = inRange(player.pos.x - player.size,
		player.pos.x + player.size, bullet.pos.x + bullet.size);
	if(bullet.xVel < 0)
		xIntersects = inRange(player.pos.x - player.size,
		player.pos.x + player.size, bullet.pos.x - bullet.size);
	yIntersects = inRange(player.pos.y - player.size,
		player.pos.y + player.size / 2, bullet.pos.y - bullet.size / 2)
		|| inRange(player.pos.y - player.size,
		player.pos.y + player.size / 2, bullet.pos.y + bullet.size / 2);

	return xIntersects && yIntersects; 
} 