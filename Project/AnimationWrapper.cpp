#include "grafix.h"
#include "AnimationWrapper.h"
#include "Game.h"
#include "Map.h"
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"

/*
 * AnimationWrapper class function
 * definitions. Written by Shane
 * Perreault 11/8/2014
 */

/**
 * AnimationWrapper constructor method
 * @param width the window width in pixels
 * @param height the window height in pixels
 */
AnimationWrapper::AnimationWrapper(int width, int height) {

	// set window width and height
	this->WINDOW_WIDTH = width;
	this->WINDOW_HEIGHT = height;

}

// returns the left x coordinate of the window
int AnimationWrapper::getLeftBoundary() {
	return (MaxX - this->WINDOW_WIDTH) / 2;
}

// returns the right x coordinate of the window
int AnimationWrapper::getRightBoundary() {
	return (MinX + this->WINDOW_WIDTH)
		+ this->getLeftBoundary();
}

// returns the top y coordinate of the window
int AnimationWrapper::getTopBoundary() {
	return (MaxY - this->WINDOW_HEIGHT) / 2;
}

// returns the bottom y coordinate of the window
int AnimationWrapper::getBottomBoundary() {
	return (MinY + this->WINDOW_HEIGHT)
		+ this->getTopBoundary();
}

// draws the game data ribbon on top of the window
void AnimationWrapper::drawGameRibbon() {
	// start by drawing the game display
	draw_rectangle(Point::Point(this->getLeftBoundary(), this->getTopBoundary()), 
		Point::Point(this->getRightBoundary(), this->getBottomBoundary()));

	// draw the game information - score and lives
	set_text_font("Courier New");
	set_text_alignment(LEFT_BOTTOM);
	draw_text(this->getLeftBoundary(), 
		this->getTopBoundary(), "PLAYER 1 SCORE:" 
			+ to_string(this->gHandler.PLAYER_SCORE));

	set_text_font("webdings");
	set_text_alignment(RIGHT_BOTTOM);
	for(int i = 0; i < this->gHandler.PLAYER_LIVES; i++) {
		draw_text(this->getRightBoundary() - i * PerX(1), this->getTopBoundary(), "Y");
	}
}

bool gameMuted = false;
bool gamePaused = false;

// handles game controls
void AnimationWrapper::runGameControls() {

	Grid controlWrapper(Point::Point(MaxX, MaxY), Point::Point(PerX(90), PerY(95)), 1, 2);
	// draw the control buttons
	string muteText = gameMuted ? "Unmute" : "Mute";
	string pauseText = gamePaused ? "Unpause" :"Pause";
	set_text_font(COURIER);
	set_text_alignment(CENTER);
	controlWrapper(0, 0).draw_text(muteText);
	controlWrapper(0, 1).draw_text(pauseText);

	// draw the grid
	controlWrapper.draw();

	// handle grid input
	if(controlWrapper.is_click()) {
		GridCell clickedCell = controlWrapper.get_click();
		// if game is being muted / unmuted
		if(clickedCell.col == 0)
			gameMuted = !gameMuted;
		// if game is being paused
		else
			gamePaused = !gamePaused;
	}
}


// pause menu grid
//int menuWidth = 180, menuHeight = 80;
//Grid pauseMenu(MidX, MidY + (menuHeight * 1/10), 
//	(2 * menuWidth / 3), (2 * menuHeight / 5), 1, 1);
// draws the pause menu
void AnimationWrapper::drawPauseMenu() {

	int menuWidth = 180, menuHeight = 80;
	Grid pauseMenu(MidX, MidY + (menuHeight * 1/10), 
		(2 * menuWidth / 3), (2 * menuHeight / 5), 1, 1);

	// draw menu container
	set_fill_color(BLACK);
	draw_rectangle_filled(MidPt, menuWidth, menuHeight);

	// draw menu title
	set_text_font("Courier New");
	set_text_alignment(CENTER);
	draw_text(MidX, MidY - (2 * menuHeight / 3), "PAUSED");

	// unpause button
	pauseMenu(0, 0).draw_text("Unpause");

	if(pauseMenu(0,0).is_click()) {
		gamePaused = false;
		pauseMenu(0,0).remove_click();
	}

	// draw the menu
	pauseMenu.draw();
}

bool spacePressed = false;
int spawnCount = 0;

// takes in user input and updates
// game accordingly
void AnimationWrapper::update(int animationPercent) {

	// update the players position
	// based on arrow movement
	if(GetAsyncKeyState(VK_LEFT)) { // move left
		this->gHandler.player.facingEast = true;
		if(this->gHandler.player.pos.x - (1.5 * this->gHandler.player.size)
			>= this->getLeftBoundary())
		this->gHandler.player.pos.x -=
			this->gHandler.player.xVel;
	}
	if(GetAsyncKeyState(VK_RIGHT)) { // move right
		this->gHandler.player.facingEast = false;
		// move the player but watch so they dont walk off screen
		if(this->gHandler.player.pos.x + (1.5 * this->gHandler.player.size)
			<= this->getRightBoundary())
			this->gHandler.player.pos.x +=
				this->gHandler.player.xVel;
	}

	// handle player jumping
	if(GetAsyncKeyState(VK_UP) && this->gHandler.player.can_jump) {
		this->gHandler.player.can_jump = false;
		if(!gameMuted)
			play_sound("jump.wav");
	}
	this->gHandler.player.jump(this->map.getFloor());

	// register when a user fires
	if(GetAsyncKeyState(VK_SPACE)) {
		if(spacePressed == false) {
			this->gHandler.player.fire();
			if(!gameMuted)
				play_sound("fire.wav");
			spacePressed = true;
		}
	}
	else {
		spacePressed = false;
	}

	// update player firing animations
	for(int i = 0; i < this->gHandler.player.shots.size(); i++) {

		// update position
		this->gHandler.player.shots[i].pos.x +=
			this->gHandler.player.shots[i].xVel;

		// check for collision on walls
		if(this->getLeftBoundary() >= this->gHandler.player.shots[i].pos.x
			- this->gHandler.player.shots[i].size ||
			this->getRightBoundary() <= this->gHandler.player.shots[i].pos.x
			+ this->gHandler.player.shots[i].size) {
			this->gHandler.player.shots.erase(
				this->gHandler.player.shots.begin() + i);
			break;
		}


		// draw the bullet
		this->gHandler.player.shots[i].drawBullet();
	}

		// handle bullet collisions
	this->gHandler.detectHits(gameMuted);

	// create enemies if under certain amount
	if(animationPercent > (27.0 / 30.0) * 100 && this->gHandler.enemies.size() < 2 && spawnCount < 5) {
		this->gHandler.spawnEnemy(this->map.getFloor(),
			this->getLeftBoundary(), this->getRightBoundary());
		spawnCount++;
	}
	else {
		spawnCount = 0;
	}

	// update the enemies positions
	// based on time
	for(int i = 0; i < this->gHandler.enemies.size(); i++) {

		// figure out enemy direction
		if(this->gHandler.player.pos.x 
			- this->gHandler.enemies[i].pos.x < 0)
			this->gHandler.enemies[i].xVel = 
			-1 * abs(this->gHandler.enemies[i].xVel);
		else
			this->gHandler.enemies[i].xVel
			= abs(this->gHandler.enemies[i].xVel);

		this->gHandler.enemies[i].pos.x 
			+= this->gHandler.enemies[i].xVel;

	}

	// update enemy firing animation
	if(animationPercent > (29.0/30.0) * 100) {
		if(this->gHandler.enemies.size() > 0) {
			// randomly choose an enemy to fire a bullet
			int i = random(0, this->gHandler.enemies.size() - 1);

			// construct bullet
			Point bulletPos = this->gHandler.enemies[i].pos;
			Color bulletColor = Color::Color(66, 168, 139);
			int bulletSize = 10;
			int bulletVel = (this->gHandler.enemies[i].xVel /
				abs(this->gHandler.enemies[i].xVel)) * 10;

			this->gHandler.enemyShots.push_back(
				Bullet::Bullet(bulletPos, bulletColor, bulletSize, bulletVel));
		}
	}

	// draw shots fired
	for(int i = 0; i < this->gHandler.enemyShots.size(); i++) {
		// update position
		this->gHandler.enemyShots[i].pos.x 
			+= this->gHandler.enemyShots[i].xVel;

		// draw bullet
		this->gHandler.enemyShots[i].drawBullet();

		// check for collision with wall
		if(this->gHandler.enemyShots[i].pos.x 
			- this->gHandler.enemyShots[i].size <= this->getLeftBoundary()
			|| this->gHandler.enemyShots[i].pos.x
			+ this->gHandler.enemyShots[i].size >= this->getRightBoundary()) {
				this->gHandler.enemyShots.erase(this->gHandler.enemyShots.begin() + i);
		}
	}

}

bool gameOver = false;

void AnimationWrapper::onCreate() {

	// game control instructions
	Grid instructions(MidPt, 180, 200, 6, 1);
	ANIMATE(2, true) {
		instructions(0, 0).draw_text("GAME CONTROLS");
		instructions(1, 0).draw_text("LEFT/RIGHT Arrow Keys Movement");
		instructions(2, 0).draw_text("UP Arrow Key Jump");
		instructions(3, 0).draw_text("SPACEBAR Shoot / Quick Respawn");
		instructions(4, 0).draw_text("p Pause Menu");
		instructions(5, 0).draw_text("PRESS SPACEBAR WHEN READY");
		instructions.draw();

		// handle spacebar press
		if(GetAsyncKeyState(VK_SPACE))
			break;
	}

	// initialize game
	while(!gameOver) {
		// create a new game instance
		this->gHandler = Game::Game();
		this->gHandler.PLAYER_SCORE = 0; // set the default game score
		this->gHandler.PLAYER_LIVES = 5;

		// create the map
		this->map = Map::Map(this->getLeftBoundary(),
			this->getRightBoundary(),
				this->getTopBoundary(),
					this->getBottomBoundary());

		// create the player
		Point playerPos = Point::Point(MidX,
			this->map.getFloor() - 30);
		this->gHandler.setPlayer(
			Player::Player(playerPos, 25, 10, 1));

		// start the game animation loop
		ANIMATE(2, true) {

			// draw the game ribbon
			this->drawGameRibbon();

			// draw the map
			this->map.drawFloor();

			// draw the player
			this->gHandler.player.drawPlayer();

			// draw the enemies
			for(int i = 0; i < this->gHandler.enemies.size(); i++)
				this->gHandler.enemies[i].drawEnemy();

			// update the player position 
			// based off arrow key presses
			if(!gamePaused)
				this->update(animation_percent());
			
			// draw the pause menu
			else if(!gameOver) {
				this->drawPauseMenu();
			}

			// handle if game is paused
			if(is_char()) {
				if(get_char() == 'p')
					gamePaused = !gamePaused;
					remove_char();
			}

			// draw the game controls
			this->runGameControls();

			if(this->gHandler.PLAYER_LIVES == 0) {
				animation_end();
			}
		}

		// start the repeat wrapper animation
		Grid repeat(MidX, MidY + PerY(10), 100, 30, 1, 2);
		ANIMATE(2, true) {

			// present user score
			set_text_size(20);
			draw_text(MidX, MidY - PerY(25), to_string(this->gHandler.PLAYER_SCORE) + " Points");
			set_text_size(12);

			// prompt user and present choices
			draw_text(MidPt, "GAME OVER. Play again?");
			repeat.draw();
			repeat(0, 0).draw_text("Yes.");
			repeat(0, 1).draw_text("No.");

			// get response
			if(repeat.is_click()) {
				// if user selects no
				if(repeat.get_click().col == 1)
					gameOver = true;
				break;
			}

			// quick respawn
			if(GetAsyncKeyState(VK_SPACE))
				break;

			// draw footnote
			draw_text(MidX, MaxY - PerY(10), "Written by Shane Perreault");
		}
	}
}