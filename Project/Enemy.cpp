#include "Enemy.h"
#include "grafix.h"

// default constructor
Enemy::Enemy() {
	// Default Enemy Color
	this->enemyColor = Color::Color(52, 201, 69);

	// Default Face Color
	this->enemyFaceColor = Color::Color(34, 173, 50);
}

// destructor
Enemy::~Enemy() {
}

// draws the enemy
void Enemy::drawEnemy() {
	set_fill_color(this->enemyColor);
	set_pen_color(this->enemyFaceColor);

	int size = this->size;
	int xSize = this->xVel < 0 ? size : size * -1;
	int x = this->pos.x;
	int y = this->pos.y - size / 4;

	// draw the head
	draw_square_filled(x, y - size, size / 2);

	// draw the mask
	set_fill_color(this->enemyFaceColor);
	draw_rectangle_filled(x, y - size, size / 2, size / 4);

	// draw the eyes
	set_fill_color(BLACK);
	draw_circle_filled(x - xSize / 4, y - (size * 12 / 11), size / 8);
	draw_circle_filled(x + xSize / 8, y - (size * 12 / 11), size / 8);

	// draw mouth
	draw_rectangle_filled(x, y - (size / 1.8), size / 8, 2);

	// draw the left arm
	set_fill_color(this->enemyColor);
	draw_square_filled(x - xSize, y + size / 8, size / 3);

	// draw the right arm
	draw_square_filled(x + xSize, y + size / 8, size / 3);


	// draw the left foot
	Point leftFootP1 = Point::Point(x - xSize, y + size);
	Point leftFootP2 = Point::Point(x + (xSize * 1/10), y + (size * 12/10));
	draw_rectangle_filled(leftFootP1, leftFootP2);

	// draw the right foot
	Point rightFootP1 = Point::Point(x + xSize, y + size);
	Point rightFootP2 = Point::Point(x + (xSize * 1/10), y + (size * 12/10));
	draw_rectangle_filled(rightFootP1, rightFootP2);

	// draw the weapon in left hand
	set_pen_color(WHITE);
	set_fill_color(WHITE);
	draw_rectangle_filled(x - 1.5 * xSize, y - size / 8, size, size / 8);
}

// kills the enemy
void Enemy::kill() {

}