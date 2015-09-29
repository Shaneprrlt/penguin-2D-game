#include "Player.h"
#include "grafix.h"

// default constructor
Player::Player() {}

// construtor for player class
Player::Player(Point pos, int size, int xVel, int yVel, int jVel, int jB) {
	// set basic data about player
	this->pos = pos;
	this->size = size;
	this->facingEast = true;
	this->xVel = xVel;
	this->yVel = yVel;
	this->jumpVelocity = jVel;
	this->jumpBoundary = jB;
	this->can_jump = true;
	this->falling = false;
	this->gravity = -3;
}


// draws the player to the screen
void Player::drawPlayer() {
	set_fill_color(WHITE);

	int size = this->size;
	int xSize = this->facingEast ? size : size * -1;
	int x = this->pos.x;
	int y = this->pos.y;

	// draw the bill
	Point billP1 = Point::Point(x - xSize, y - size / 2);
	Point billP2 = Point::Point(x + xSize / 2, y - size);
	draw_rectangle_filled(billP1, billP2);

	// draw the head
	draw_square(x, y - size, size / 2);

	// draw the eyes
	draw_circle_filled(x - xSize / 4, y - (size * 12 / 11), size / 8);
	draw_circle_filled(x + xSize / 8, y - (size * 12 / 11), size / 8);

	// draw the left arm
	draw_square(x - xSize, y, size / 2);

	// draw the right arm
	draw_square(x + xSize, y, size / 2);

	// draw the left foot
	Point leftFootP1 = Point::Point(x - xSize, y + size);
	Point leftFootP2 = Point::Point(x + (xSize * 1/10), y + (size * 12/10));
	draw_rectangle_filled(leftFootP1, leftFootP2);

	// draw the right arm
	Point rightFootP1 = Point::Point(x + xSize, y + size);
	Point rightFootP2 = Point::Point(x + (xSize * 1/10), y + (size * 12/10));
	draw_rectangle_filled(rightFootP1, rightFootP2);
}


// makes the player jump
void Player::jump(int returnY) {
	if(!this->can_jump) { // if in a jump cycle
		// if player is still in rising part of jump
		if(!this->falling) {
			this->pos.y -= this->jumpVelocity - this->gravity;
			this->gravity++;
		}
		// if player is in falling part of jump
		if(this->falling) {
			this->pos.y += this->jumpVelocity;
		}
		// check to see if player has hit the boundary
		if(this->pos.y <= returnY - this->jumpBoundary + 20
			|| (this->jumpVelocity - this->gravity) <= 0) {
			this->falling = true;
			this->gravity = -3;
		}
		// check to see if player has hit the floor
		if(this->pos.y + (this->size * 12/10) >= returnY && this->falling) {
			this->pos.y = returnY - (this->size * 12/10);
			this->can_jump = true;
			this->falling = false;
		}
	}
}

// makes the player fire a shot
void Player::fire() {
	int xVel = this->facingEast ? - 20 : 20;
	this->shots.push_back(Bullet::Bullet(Point::Point(
		this->pos.x, this->pos.y), WHITE, (int) this->size / 2.5, xVel));
}