#include "Bullet.h"
#include "grafix.h"

// default constructor
Bullet::Bullet() {}

// constructor
Bullet::Bullet(Point pos, Color color, int size, int xVel) {
	this->pos = pos;
	this->color = color;
	this->size = size;
	this->xVel = xVel;
}

// destructor
Bullet::~Bullet() {}

// draws the bullet to the screen
void Bullet::drawBullet() {
	
	// get points and draw the bullet
	set_fill_color(this->color);
	draw_rectangle_filled(this->pos.x, this->pos.y, size, size / 2);
	set_fill_color(WHITE);
}
