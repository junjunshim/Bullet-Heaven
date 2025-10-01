#pragma once

#include "framework.h"

/// <summary>
/// Player Ŭ����
/// obj => ��ü �����¿� ��ǥ ����
/// speed => �������� ���� �� ��ü �̵��ӵ�
/// width => ��ü �ʺ�
/// height => ��ü ����
/// x, y => �������� ��꿡 ���� ���� ��ǥ
/// vx, vy => x�� , y�� ���� �ӵ�
/// ax, ay => x�� , y�� ���� ���ӵ�
/// moveForce => ���ӵ� ũ��
/// friction => ���� ���
/// dashForce => �뽬 ũ��
/// </summary>
class Player
{
private:
	RECT obj;
	int speed;
	int width;
	int height;

	float x, y;
	float vx, vy;
	float ax, ay;
	float moveForce;
	float friction;
	float dashForce;
public:
	Player();
	
	// obj �ּ� getter
	PRECT getObj();

	// obj �⺻ ��ġ ����(left, top)
	void setObj(int, int);

	// obj �ʺ�, ���� setter
	void setWidth(int);
	void setHeight(int);

	// obj �ʺ�, ���� getter
	int getWidth();
	int getHeight();

	// ���� ��ǥ ����
	void setLeft(int);
	void setTop(int);
	void setRight(int);
	void setBottom(int);

	// ��ǥ getter
	int getLeft();
	int getTop();
	int getRight();
	int getBottom();

	// ��ǥ �̵�
	void moveLeft();
	void moveTop();
	void moveRight();
	void moveBottom();

	//��ǥ �̵�(�̵� ����)
	void moveLeft(RECT);
	void moveTop(RECT);
	void moveRight(RECT);
	void moveBottom(RECT);

	// speed setter
	void setSpeed(int);

	// ���������� 
	void update(RECT);
	// ���� ���ӵ� ����
	void applyForce(float, float);
	// moveForce��ŭ ����
	void applyForceLeft();
	void applyForceRight();
	void applyForceTop();
	void applyForceBottom();
	// �뽬 ��� �� ���ӵ� ����
	void applyDashForce();
	// moveForce setter, getter
	void setMoveForce(float);
	float getMoveForce();
};

