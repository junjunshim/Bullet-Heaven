// Bullet-Heaven.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "Bullet-Heaven.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <vector>
#include "Player.h"
#include "myCommand.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT2, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT2));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT2));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT2);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//

// 오프젝트 생성
Player user;
std::vector<Player> tangs;
std::vector<Player> foods;

// 전체 사용 맵
RECT map = { 10,10,1000,800 };

// 게임 시작 버튼 위치
RECT startButton = {
    ((map.right - map.left) / 2 - 70),
    (map.top + 300),
    ((map.right - map.left) / 2 + 70),
    (map.top + 300 + 50)
};

// 키다운 확인
bool isLeftPressed = false;
bool isRightPressed = false;
bool isUpPressed = false;
bool isDownPressed = false;

// 게임 타이머
int gameTime = 0;

// 스코어 및 기본 점수
int score = 0;
int point = 25;

// 대쉬용 변수
int dashCoolTime = 20;
int dashCount = 0;

// 마우스 위치
int g_x = 0;
int g_y = 0;

// 게임 상태 플래그 변수
bool isMainMenu, isGamePlaying, isGameOver;

// 게임 로직 공용 함수 선언
void normalization(float*, float*); // 방향 벡터 정규화 함수
void initGame(); // 게임 시작 시, 초기화 담당 함수
void changeGameStatus(int); // 게임 상태 변경 함수

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        // 난수 초기화
        srand(time(NULL));
        // 게임 디폴트 상태(메인 메뉴)
        changeGameStatus(MAIN_MENU);
    }
    break;
    case WM_TIMER:
    {
        switch (wParam) {
        case FRAME_60FPS:
        {
            // 유저 위치 이동 로직
            float m_x = 0.0f;
            float m_y = 0.0f;
            if (isLeftPressed) {
                m_x -= 1;
            }
            if (isRightPressed) {
                m_x += 1;
            }
            if (isUpPressed) {
                m_y -= 1;
            }
            if (isDownPressed) {
                m_y += 1;
            }

            // 대각선 이동 시, 속도 빨라지는 것 방지(정규화)
            normalization(&m_x, &m_y);

            user.applyForce(m_x * USER_FORCE, m_y * USER_FORCE);

            user.update(map);

            // tang들 간의 충돌 검사 로직
            for (int i = 0; i < tangs.size(); i++) {
                // tang의 상태 확인 => 잡힌 상태면, 충돌 검사 X
                if (tangs[i].getIsHold()) {
                    continue;
                }
                // tang의 충돌검사는 인덱스가 큰 tang들만 검사하여 여러 번 검사하는 것을 방지
                for (int j = i + 1; j < tangs.size(); j++) {
                    // 비교를 할 tang가 잡힌 상태면 충돌 검사 X
                    if (tangs[j].getIsHold()) {
                        continue;
                    }

                    RECT ret;
                    // tang 간의 충돌 비교(IntersectRect() 함수 사용)
                    if (IntersectRect(&ret, tangs[i].getObj(), tangs[j].getObj())) {
                        // 충돌 시, 어느 방향으로 튕겨나가야 할지 계산
                        // ex) dirX = a.left - b.left => dirX는 a가 b로 향하는 x축 방향 벡터 값★★★★ 
                        float dirX = tangs[j].getLeft() - tangs[i].getLeft();
                        float dirY = tangs[j].getTop() - tangs[i].getTop();

                        // 계산한 방향 벡터를 정규화
                        normalization(&dirX, &dirY);

                        // 각 tang에 가속도에 방향 벡터 값 추가 
                        tangs[i].applyForce(-dirX * TANG_CRASH_FORCE, -dirY * TANG_CRASH_FORCE);
                        tangs[j].applyForce(dirX * TANG_CRASH_FORCE, dirY * TANG_CRASH_FORCE);

                        // tang의 물리 상태 업데이트
                        tangs[i].update(map);
                        tangs[j].update(map);
                    }
                }
            }

            // tang의 user 추적 로직
            for (int i = 0; i < tangs.size(); i++) {
                // tang가 잡힌 상태면, X
                if (tangs[i].getIsHold()) {
                    continue;
                }

                // tangs[i]에서 user를 향하는 방향 벡터 계산
                // ex) dirX = a.left - b.left => dirX는 a가 b로 향하는 x축 방향 벡터 값★★★★
                float dirX = user.getLeft() - tangs[i].getLeft();
                float dirY = user.getTop() - tangs[i].getTop();

                // 계산한 방향 벡터를 정규화
                normalization(&dirX, &dirY);

                // tang에게 정규화된 방향으로 힘을 가함
                int increasedTime = score / TANG_UNIT_SCORE;
                float tangForce = TANG_DEFAULT_CHASE_FORCE + (TANG_ADDITIONAL_CHASE_FORCE * increasedTime);
                tangs[i].applyForce(dirX * tangForce, dirY * tangForce);

                // tang의 물리 상태 업데이트
                tangs[i].update(map);
            }

            // tang가 isHold == true일때, 마우스를 따라가도록(tang들 간의 충돌은 무시)
            for (int i = 0; i < tangs.size(); i++) {
                if (!tangs[i].getIsHold()) {
                    continue;
                }
                //tang 중앙으로 위치 보정
                // g_x, g_y => 마우스 위치
                // tang의 중앙 위치 값 구한 뒤, 방향 벡터 구하기
                int center_x = tangs[i].getLeft() + tangs[i].getWidth() / 2;
                int center_y = tangs[i].getTop() + tangs[i].getHeight() / 2;

                float dirX = g_x - center_x;
                float dirY = g_y - center_y;

                // 계산한 방향 벡터를 정규화
                normalization(&dirX, &dirY);

                // tang에게 정규화된 방향으로 힘을 가함
                tangs[i].applyForce(dirX * TANG_HOLD_FORCE, dirY * TANG_HOLD_FORCE);

                // tang의 물리 상태 업데이트
                tangs[i].update(map);
            }


            // user 와 food의 충돌 확인 및 food 위치 재배치
            // score 증가 및 tang의 속도 증가(score에 의해 변동)
            // score 일정치 이상 증가 시, tang 증가 로직
            RECT ret_food;
            for (int i = 0; i < foods.size(); i++) {
                if (IntersectRect(&ret_food, user.getObj(), foods[i].getObj())) {
                    score += point;
                    RECT re;
                    do {
                        foods[i].setObj(rand() % (map.right - foods[i].getWidth() - map.left) + map.left, rand() % (map.bottom - foods[i].getHeight() - map.top) + map.top);
                    } while (IntersectRect(&re, user.getObj(), foods[i].getObj()));
                    // 100점마다 tang 증가 로직 + food 증가 로직
                    if (score % 100 == 0) {
                        // tang 증가
                        Player newTang;
                        RECT result;
                        /*
                            랜덤 위치 생성 로직
                            1. rand() 함수를 통해서 램덤 위치 생성, 이때 전체 맵 크기와 tang의 넓이를 계산하여 랜덤 값 생성
                            2. do while()를 이용하여 위치 생성 이후, 유저와 겹쳐서 생성 시, 다시 위치 생성
                            3. 조건 만족 시, 여러 tang들을 관리하기 위한 tangs(vector)에 push_back
                        */
                        do {
                            newTang.setObj(rand() % (map.right - newTang.getWidth() - map.left) + map.left, rand() % (map.bottom - newTang.getHeight() - map.top) - map.top);
                        } while (IntersectRect(&result, user.getObj(), newTang.getObj()));
                        tangs.push_back(newTang);

                        // food 증가
                        Player newFood;
                        newFood.setWidth(30);
                        newFood.setHeight(30);
                        do {
                            newFood.setObj(rand() % (map.right - newFood.getWidth() - map.left) + map.left, rand() % (map.bottom - newFood.getHeight() - map.top) + map.top);
                        } while (IntersectRect(&result, user.getObj(), newFood.getObj()));
                        foods.push_back(newFood);
                    }
                }
            }

            // user와 tang들 충돌 검사 ()
            for (int i = 0; i < tangs.size(); i++) {
                // user 와 tang의 충돌 확인 및 타이머 해제
                RECT ret_tang;
                if (IntersectRect(&ret_tang, user.getObj(), tangs[i].getObj())) {
                    KillTimer(hWnd, FRAME_60FPS);
                    KillTimer(hWnd, ONE_SECOND);
                    changeGameStatus(GAME_OVER);
                    MessageBox(hWnd, L"Game Over", L"Info", MB_OK);
                    changeGameStatus(INIT_USER_ARROW_KEY);
                    changeGameStatus(MAIN_MENU);
                }
            }

            // 모든 로직 수행 후, 화면 재구성()
            InvalidateRect(hWnd, NULL, FALSE);
        }
        break;
        case ONE_SECOND:
        {
            if (isGamePlaying) {
                // 타이머 증가 로직 + 대수 쿨타임 계산
                gameTime++;
                if (dashCount > 0) {
                    dashCount--;
                }
            }
        }
        break;
        default:
        {
        }
        break;
        }
    }
    break;
    case WM_LBUTTONDOWN:
    {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);
        RECT mousePoint = { x, y ,x + 1, y + 1 };
        // 게임 상태가 isMainMenu일때
        if (isMainMenu) {
            RECT result;
            if (IntersectRect(&result, &startButton, &mousePoint)) {
                // 게임 시작 버튼 클릭 시,  게임 초기화 위치
                initGame();
                // 게임 상태 초기화
                changeGameStatus(GAME_PLAYING);
                // 타이머 다시 세팅
                SetTimer(hWnd, FRAME_60FPS, 16, NULL);
                SetTimer(hWnd, ONE_SECOND, 1000, NULL);
            }
        }
        if (isGamePlaying) {
            // 마우스 왼쪽 클릭 시) tang가 잡혔는 지 확인
            RECT mousePoint = { x, y, x + 1, y + 1 };
            for (int i = 0; i < tangs.size(); i++) {
                RECT result;
                if (IntersectRect(&result, tangs[i].getObj(), &mousePoint)) {
                    // tang의 isHold 상태 변경
                    tangs[i].TrueIsHold();
                    break; // 하나의 tang만 잡기 위해 break ★★★★★
                }
            }
        }
    }
    break;
    case WM_LBUTTONUP:
    {
        if (isGamePlaying) {
            for (int i = 0; i < tangs.size(); i++) {
                tangs[i].FalseIsHold();
            }
        }
    }
    break;
    case WM_MOUSEMOVE:
    {
        if (isGamePlaying) {
            // 마우스 포인터 위치 저장
            g_x = LOWORD(lParam);
            g_y = HIWORD(lParam);
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            RECT mousePoint = { x, y, x + 1, y + 1 };
            if (isGamePlaying) {
                // 마우스가 움직여서 tang를 놓쳤는 지 확인
                for (int i = 0; i < tangs.size(); i++) {
                    RECT result;
                    RECT tangCorrectionArea = { tangs[i].getLeft() - 20,tangs[i].getTop() - 20,tangs[i].getRight() + 20,tangs[i].getBottom() + 20 };
                    if (!IntersectRect(&result, &tangCorrectionArea, &mousePoint)) {
                        // tang의 isHold 상태 변경
                        tangs[i].FalseIsHold();
                    }
                }
            }
        }
    }
    break;
    case WM_KEYDOWN:
    {
        switch (wParam) {
            // 유저가 어떤 방향 키를 입력하는 지, 확인
        case 'A':
        {
            if (isGamePlaying) {
                isLeftPressed = true;
            }
        }
        break;
        case 'D':
        {
            if (isGamePlaying) {
                isRightPressed = true;
            }
        }
        break;
        case 'W':
        {
            if (isGamePlaying) {
                isUpPressed = true;
            }
        }
        break;
        case 'S':
        {
            if (isGamePlaying) {
                isDownPressed = true;
            }
        }
        break;
        case VK_SPACE:
        {
            // 스페이스 입력 시, 대쉬 스킬이 나가도록 설정(dashCount 0일때만, 적용)
            if (isGamePlaying) {
                if (dashCount == 0) {
                    user.applyDashForce();
                    user.update(map);
                    dashCount = dashCoolTime;
                }
            }
        }
        break;
        case VK_F1:
        {
        }
        break;
        default:
        {
        }
        break;
        }
    }
    break;
    case WM_KEYUP:
    {
        switch (wParam) {
        case 'A':
        {
            if (isGamePlaying) {
                isLeftPressed = false;
            }
        }
        break;
        case 'D':
        {
            if (isGamePlaying) {
                isRightPressed = false;
            }
        }
        break;
        case 'W':
        {
            if (isGamePlaying) {
                isUpPressed = false;
            }
        }
        break;
        case 'S':
        {
            if (isGamePlaying) {
                isDownPressed = false;
            }
        }
        break;
        default:
        {
        }
        }
    }
    break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // 메뉴 선택을 구문 분석합니다:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        // hdc 에는 그림을 그릴 대상과 연결되어있다.
        // (설정된 값과 도구의 모음), (그려진 그림의 데이터는 가지고 있지 않는다.)

        // 더블 버퍼링 구현(중요)
        // 변수 선언
        HDC hMemDC;
        HBITMAP hBitmap;
        HBITMAP hOldBitmap;
        HPEN myPen = 0;
        HPEN oldPen;
        HBRUSH myBrush = 0;
        HBRUSH oldBrush;
        HFONT myFont = 0;
        HFONT oldFont;
        RECT rect;

        // 클라이언트 영역 얻어오기
        GetClientRect(hWnd, &rect);

        // hMemDC에 메모리용 DC 생성
        hMemDC = CreateCompatibleDC(hdc);

        // 클라이언트랑 동일한 크기의 비트맵 생성
        hBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);

        // hMemDC에 hBitmap 비트맵을 사용하고 (hMemDC로 그림을 그릴 대상 => hBitmap)
        // 사용하던 비트맵을 hOldBitmap에 저장한다
        hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

        // 백버퍼를 하얀색으로 칠한다
        PatBlt(hMemDC, 0, 0, rect.right, rect.bottom, WHITENESS);

        // 백버퍼에서 그리기

        // 게임 상태에 상관없이 항상 그리는 부분
        Rectangle(hMemDC, map.left, map.top, map.right, map.bottom);

        // 게임 상태가 메인 메뉴 일때, 그리는 부분
        if (isMainMenu) {
            // 메인 메뉴 텍스트 => 가운데 정렬
            SetTextAlign(hMemDC, TA_CENTER);
            // 타이틀 폰트 50px
            myFont = CreateFont(50, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                DEFAULT_PITCH | FF_SWISS, L"Arial");
            oldFont = (HFONT)SelectObject(hMemDC, myFont);
            // 게임 타이틀
            WCHAR gameMainTitle[30] = L"Bullet-Heaven";
            TextOut(hMemDC, map.left + ((map.right - map.left) / 2), map.top + 100, gameMainTitle, lstrlen(gameMainTitle));

            // 게임 시작 버튼 && start 텍스트
            myFont = CreateFont(30, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                DEFAULT_PITCH | FF_SWISS, L"Arial");
            SelectObject(hMemDC, myFont);
            Rectangle(hMemDC, startButton.left, startButton.top, startButton.right, startButton.bottom);
            WCHAR startButtonText[10] = L"STRAT";
            TextOut(hMemDC, startButton.left + ((startButton.right - startButton.left) / 2), startButton.top + 10, startButtonText, lstrlen(startButtonText));
            SelectObject(hMemDC, oldFont);
            SetTextAlign(hMemDC, TA_LEFT);
        }

        // 게임 상태가 진행 중 일때, 그리는 부분
        if (isGamePlaying) {
            int textV_gap = 10;

            myFont = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                DEFAULT_PITCH | FF_SWISS, L"Arial");
            oldFont = (HFONT)SelectObject(hMemDC, myFont);
            // 게임 타이머
            WCHAR timerText[100];
            wsprintfW(timerText, L"Time : %d", gameTime);
            TextOut(hMemDC, map.right + 10, map.top + textV_gap, timerText, lstrlenW(timerText));
            textV_gap += 20;

            // 게임 스코어
            WCHAR scoreText[100];
            wsprintfW(scoreText, L"Score : %d", score);
            TextOut(hMemDC, map.right + 10, map.top + textV_gap, scoreText, lstrlenW(scoreText));
            textV_gap += 20;

            // 대쉬
            WCHAR dashText[50];
            if (dashCount == 0) {
                wsprintfW(dashText, L"Dash Ready!");
            }
            else {
                wsprintfW(dashText, L"Dash CoolTime : %d", dashCount);
            }
            TextOut(hMemDC, map.right + 10, map.top + textV_gap, dashText, lstrlenW(dashText));
            textV_gap += 20;

            SelectObject(hMemDC, oldFont);

            // user 위치
            Rectangle(hMemDC, user.getLeft(), user.getTop(), user.getRight(), user.getBottom());

            // tangs 위치
            for (int i = 0; i < tangs.size(); i++) {
                if (!tangs[i].getIsHold()) {
                    Ellipse(hMemDC, tangs[i].getLeft(), tangs[i].getTop(), tangs[i].getRight(), tangs[i].getBottom());
                }
                else {
                    myBrush = CreateSolidBrush(RGB(0, 20, 150));
                    oldBrush = (HBRUSH)SelectObject(hMemDC, myBrush);
                    Ellipse(hMemDC, tangs[i].getLeft(), tangs[i].getTop(), tangs[i].getRight(), tangs[i].getBottom());
                    myBrush = (HBRUSH)SelectObject(hMemDC, oldBrush);
                }
            }

            // foods 위치
            for (int i = 0; i < foods.size(); i++) {
                Ellipse(hMemDC, foods[i].getLeft(), foods[i].getTop(), foods[i].getRight(), foods[i].getBottom());
            }


            // enemy 게이지 바
            Rectangle(hMemDC, map.left, map.bottom, map.right, map.bottom + 10); // 전체 게이지 바
            // 게이지 간격
            for (int i = 1; i <= 4; i++) {
                int gaugeGap = (map.right - map.left) / 5;
                MoveToEx(hMemDC, map.left + gaugeGap * i, map.bottom, NULL);
                LineTo(hMemDC, map.left + gaugeGap * i, map.bottom + 10);
            }
            // 
            myPen = CreatePen(PS_NULL, NULL, NULL);
            oldPen = (HPEN)SelectObject(hMemDC, myPen);
            myBrush = CreateSolidBrush(RGB(255, 0, 0));
            oldBrush = (HBRUSH)SelectObject(hMemDC, myBrush);
            int enemyGauge = map.left;
            // 게이지 전체는 100, 게이지 간격 20, 20당 구간 길이 => (map.right - map.left) / 5
            enemyGauge += (score % 100 / 20) * (map.right - map.left) / 5;
            Rectangle(hMemDC, map.left, map.bottom, enemyGauge, map.bottom + 10);

            SelectObject(hMemDC, oldPen);
            SelectObject(hMemDC, oldBrush);

            // 게임 상태가 게임 오버 일때, 그리는 부분
            if (isGameOver) {
                WCHAR overText[50] = L"F1 입력 시, 다시 시작";
                TextOut(hMemDC, map.right + 10, map.top + 70, overText, lstrlenW(overText));
            }
        }

        // 백버퍼에 그려진 내용을 hdc로 복사
        BitBlt(hdc, 0, 0, rect.right, rect.bottom, hMemDC, 0, 0, SRCCOPY);

        // 리소스 해제
        SelectObject(hMemDC, hOldBitmap);
        DeleteObject(myPen);
        DeleteObject(myBrush);
        DeleteObject(myFont);
        DeleteObject(hBitmap);
        DeleteObject(hMemDC);

        // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
    {
        //DestroyWindow(g_button);
        tangs.clear();
        foods.clear();
        PostQuitMessage(0);
    }
    break;
    default:
    {
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    break;
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

// 게임 로직 공용 함수 정의
void initGame() {
    // 기존 tang와 food 들 정리
    tangs.clear();
    foods.clear();
    // 유저 위치 이동 && 속도 초기화
    // 유저 시작 위치 => 맵 정중앙
    int user_x = ((map.right - map.left) / 2) - (user.getWidth() / 2);
    int user_y = ((map.bottom - map.top) / 2) - (user.getHeight() / 2);
    user.setObj(user_x, user_y);
    user.setVx(0.0);
    user.SetVy(0.0);
    // 게임 시간, 대쉬 쿨타임, 스코어 초기화
    gameTime = 0;
    dashCount = 0;
    score = 0;

    // 적 (default width, heigth => 50)
    for (int i = 0; i < STARTENEMY; i++) {
        Player newTang;
        RECT result;
        /*
            랜덤 위치 생성 로직
            1. rand() 함수를 통해서 램덤 위치 생성, 이때 전체 맵 크기와 tang의 넓이를 계산하여 랜덤 값 생성
            2. do while()를 이용하여 위치 생성 이후, 유저와 겹쳐서 생성 시, 다시 위치 생성
            3. 조건 만족 시, 여러 tang들을 관리하기 위한 tangs(vector)에 push_back
        */
        do {
            newTang.setObj(rand() % (map.right - newTang.getWidth() - map.left) + map.left, rand() % (map.bottom - newTang.getHeight() - map.top) - map.top);
        } while (IntersectRect(&result, user.getObj(), newTang.getObj()));
        tangs.push_back(newTang);
    }

    // 4, 음식
    Player newFood;
    newFood.setWidth(30);
    newFood.setHeight(30);
    // tang와 똑같이 랜덤 위치 생성, 시작 생성 위치가 유저와 겹치면 다시 생성
    RECT result;
    do {
        newFood.setObj(rand() % (map.right - newFood.getWidth() - map.left) + map.left, rand() % (map.bottom - newFood.getHeight() - map.top) + map.top);
    } while (IntersectRect(&result, user.getObj(), newFood.getObj()));
    foods.push_back(newFood);
}
void changeGameStatus(int status) {
    switch (status) {
    case MAIN_MENU:
    {
        isMainMenu = true;
        isGamePlaying = false;
        isGameOver = false;
    }
    break;
    case GAME_PLAYING:
    {
        isMainMenu = false;
        isGamePlaying = true;
        isGameOver = false;
    }
    break;
    case GAME_STOP:
    {
    }
    break;
    case GAME_OVER:
    {
        isGameOver = false;
    }
    break;
    case INIT_USER_ARROW_KEY:
    {
        isDownPressed = false;
        isLeftPressed = false;
        isRightPressed = false;
        isUpPressed = false;
    }
    break;
    default:
    {
    }
    break;
    }
}
void normalization(float* x, float* y) {
    float distance = sqrt((*x) * (*x) + (*y) * (*y));
    if (distance != 0) {
        *x /= distance;
        *y /= distance;
    }
}