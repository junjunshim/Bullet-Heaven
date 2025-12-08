#pragma once

#include "framework.h"

/// <summary>
/// Player 클래스
/// obj => 객체 상하좌우 좌표 저장
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
	int width;
	int height;

	float x, y;
	float vx, vy;
	float ax, ay;
	float moveForce;
	float friction;
	float dashForce;

	bool isHold;
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
	// vx, vy setter, getter
	void setVx(float);
	void SetVy(float);
	float getVx();
	float getVy();
	// 마우스 처리용
	bool getIsHold();
	void TrueIsHold();
	void FalseIsHold();
};

