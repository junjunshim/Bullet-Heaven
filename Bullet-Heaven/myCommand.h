#pragma once

// Timer 식별자
#define FRAME_60FPS 1
#define ONE_SECOND 2

// startTangNumber
#define STARTENEMY 1

// user 가속도 지정
#define USER_FORCE 0.5

// tang 관련 가속도 지정
#define TANG_CRASH_FORCE 1 // 충돌 force
#define TANG_DEFAULT_CHASE_FORCE 0.1 // 디폴트 추격 가속도
#define TANG_UNIT_SCORE 100 // 50점당 추가 가속도 증가
#define TANG_ADDITIONAL_CHASE_FORCE 0.015 // 점수 당 추가 추격 가속도
#define TANG_HOLD_FORCE 0.3 // 마우스로 끌려가는 가속도

// bullet 관련 상수
#define BULLET_SIZE 10
#define BULLET_SPEED 5
#define BULLET_DEFAULT_TIME 5
#define BULLET_UNIT_SCORE 500
#define BULLET_REDUCED_TIME 1
#define BULLET_TIME_MIN 1

// 게임 상태 상수
#define MAIN_MENU 1
#define GAME_PLAYING 2
#define GAME_STOP 3
#define GAME_OVER 4
#define INIT_USER_ARROW_KEY 10
#define DEBUG_MODE 11