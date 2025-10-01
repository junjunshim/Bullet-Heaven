#include "Player.h"
#include <math.h>

Player::Player() {
	obj = { 0, };
	speed = 1;
	width = 70;
	height = 70;

	// ���������� ����
	x = 0.0f;
	y = 0.0f;
	vx = 0.0f;
	vy = 0.0f;
	ax = 0.0f;
	ay = 0.0f;

	moveForce = 0.5f;
	friction = 0.95f;
	dashForce = 10.0f;
}

// RECT ������ ��ȯ
PRECT Player::getObj() {
	return &obj;
}

// obj �⺻ ��ġ ����(left, top)
void Player::setObj(int left, int top) {
	this->x = (float)left;
	this->y = (float)top;
	this->obj.left = left;
	this->obj.top = top;
	this->obj.right = left + this->width;
	this->obj.bottom = top + this->height;
}


// obj �ʺ�, ���� setter
void Player::setWidth(int width) {
	this->width = width;
}
void Player::setHeight(int height) {
	this->height = height;
}


// obj �ʺ�, ���� getter
int Player::getWidth() {
	return this->width;
}
int Player::getHeight() {
	return this->height;
}


// obj ��ǥ setter(���� ��ǥ ����)  (���� �Ⱦ�)
void Player::setLeft(int num) {
	this->obj.left = num;
}
void Player::setTop(int num) {
	this->obj.top = num;
}
void Player::setRight(int num) {
	this->obj.right = num;
}
void Player::setBottom(int num) {
	this->obj.bottom = num;
}


// obj ��ǥ getter
int Player::getLeft() {
	return this->obj.left;
}

int Player::getTop() {
	return this->obj.top;
}

int Player::getRight() {
	return this->obj.right;
}

int Player::getBottom() {
	return this->obj.bottom;
}

// obj ��ǥ �̵�(speed�� ����)
void Player::moveLeft() {
	this->obj.left -= this->speed;
	this->obj.right -= this->speed;
}

void Player::moveRight() {
	this->obj.left += this->speed;
	this->obj.right += this->speed;
}

void Player::moveTop() {
	this->obj.top -= this->speed;
	this->obj.bottom -= this->speed;
}

void Player::moveBottom() {
	this->obj.top += this->speed;
	this->obj.bottom += this->speed;
}

// �̵� ������ ����
// obj ��ǥ �̵�(speed�� ����)
void Player::moveLeft(RECT map) {
	this->obj.left -= this->speed;
	this->obj.right -= this->speed;
	if (this->obj.left < map.left) {
		this->obj.left = map.left;
		this->obj.right = this->obj.left + this->width;
	}
}

void Player::moveRight(RECT map) {
	this->obj.left += this->speed;
	this->obj.right += this->speed;
	if (this->obj.right > map.right) {
		this->obj.right = map.right;
		this->obj.left = this->obj.right - this->width;
	}
}

void Player::moveTop(RECT map) {
	this->obj.top -= this->speed;
	this->obj.bottom -= this->speed;
	if (this->obj.top < map.top) {
		this->obj.top = map.top;
		this->obj.bottom = this->obj.top + this->height;
	}
}

void Player::moveBottom(RECT map) {
	this->obj.top += this->speed;
	this->obj.bottom += this->speed;
	if (this->obj.bottom > map.bottom) {
		this->obj.bottom = map.bottom;
		this->obj.top = this->obj.bottom - this->height;
	}
}

// speed setter
void Player::setSpeed(int num) {
	this->speed = num;
}

// ����������
// ���� ���ӵ� ����
void Player::applyForce(float fx, float fy) {
	this->ax += fx;
	this->ay += fy;
}

// moveForce��ŭ ���ӵ� ����
void Player::applyForceLeft() {
	this->ax -= this->moveForce;
	this->ay += 0.0f;
}

void Player::applyForceRight() {
	this->ax += this->moveForce;
	this->ay += 0.0f;
}

void Player::applyForceTop() {
	this->ax += 0.0f;
	this->ay -= this->moveForce;
}

void Player::applyForceBottom() {
	this->ax += 0.0f;
	this->ay += this->moveForce;
}

// �뽬 ���ӵ� ����
void Player::applyDashForce() {
	// ���� obj�� ���� ���ϱ�
	float dirX = 0.0f;
	float dirY = 0.0f;
	float distance = sqrt(this->vx * this->vx + this->vy * this->vy);
	if (distance != 0) {
		dirX = this->vx / distance;
		dirY = this->vy / distance;
	}
	this->ax += dirX * this->dashForce;
	this->ay += dirY * this->dashForce;
}

// �ӵ��� ���� obj ��ġ ����
void Player::update(RECT map) {
	// �ӵ� ����(���ӵ� ��ŭ)
	this->vx += this->ax;
	this->vy += this->ay;

	// ������ ����
	this->vx *= this->friction;
	this->vy *= this->friction;

	// ���� �Ÿ�
	this->x += this->vx;
	this->y += this->vy;

	// ���ӵ� �ʱ�ȭ
	this->ax = 0.0f;
	this->ay = 0.0f;

	// �� �浹 �� ó��
	if (this->x < map.left) {
		this->x = map.left;
		this->vx *= -0.5f;
	}
	if (this->x + this->width > map.right) {
		this->x = map.right - width;
		this->vx *= -0.5f;
	}
	if (this->y < map.top) {
		this->y = map.top;
		this->vy *= -0.5f;
	}
	if (this->y + this->height > map.bottom) {
		this->y = map.bottom - this->height;
		this->vy *= -0.5f;
	}

	this->obj.left = (int)this->x;
	this->obj.top = (int)this->y;
	this->obj.right = (int)this->x + width;
	this->obj.bottom = (int)this->y + height;
}

// moveForce setter
void Player::setMoveForce(float force) {
	this->moveForce = force;
}

// moveForce getter
float Player::getMoveForce() {
	return this->moveForce;
}

