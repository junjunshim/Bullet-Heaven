#pragma once

#include "framework.h"

/// <summary>
/// Player 클래스
/// obj => 객체 상하좌우 좌표 저장
/// speed => 물리엔진 적용 전 객체 이동속도
/// width => 객체 너비
/// height => 객체 높이
/// x, y => 물리엔진 계산에 사용될 실제 좌표
/// vx, vy => x축 , y축 방향 속도
/// ax, ay => x축 , y축 방향 가속도
/// moveForce => 가속도 크기
/// friction => 마찰 계수
/// dashForce => 대쉬 크기
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
	
	// obj 주소 getter
	PRECT getObj();

	// obj 기본 위치 생성(left, top)
	void setObj(int, int);

	// obj 너비, 높이 setter
	void setWidth(int);
	void setHeight(int);

	// obj 너비, 높이 getter
	int getWidth();
	int getHeight();

	// 직접 좌표 지정
	void setLeft(int);
	void setTop(int);
	void setRight(int);
	void setBottom(int);

	// 좌표 getter
	int getLeft();
	int getTop();
	int getRight();
	int getBottom();

	// 좌표 이동
	void moveLeft();
	void moveTop();
	void moveRight();
	void moveBottom();

	//좌표 이동(이동 제약)
	void moveLeft(RECT);
	void moveTop(RECT);
	void moveRight(RECT);
	void moveBottom(RECT);

	// speed setter
	void setSpeed(int);

	// 물리엔진용 
	void update(RECT);
	// 지정 가속도 증가
	void applyForce(float, float);
	// moveForce만큼 증가
	void applyForceLeft();
	void applyForceRight();
	void applyForceTop();
	void applyForceBottom();
	// 대쉬 사용 시 가속도 증가
	void applyDashForce();
	// moveForce setter, getter
	void setMoveForce(float);
	float getMoveForce();
};

