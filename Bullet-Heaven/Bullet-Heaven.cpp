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
std::vector<Player> bullets;

// 전체 사용 맵
RECT map = { 10,10,1200,800 };

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
int dashCoolTime = 10;
int dashCount = 0;

// 마우스 위치
int g_x = 0;
int g_y = 0;

// 게임 상태 플래그 변수
bool isMainMenu, isGamePlaying, isGameOver;
bool isDebugMode = false;

// 게임 로직 공용 함수 선언
void normalization(float*, float*); // 방향 벡터 정규화 함수
void initGame(); // 게임 시작 시, 초기화 담당 함수
void changeGameStatus(int); // 게임 상태 변경 함수

// 상호배제 진행 => mutex 사용
// user로직과 tangs의 로직 각각 스레드로 변경
// + WM_PAINT에서도 상호배제 필요
HANDLE g_mux;
HANDLE hUserThread;
HANDLE hTangThread;
HANDLE hBulletThread;

// user스레드
DWORD WINAPI UserLogic(LPVOID param) {
    HWND hWnd = (HWND)param;
    while (isGamePlaying && !isGameOver) {
        Sleep(16);
        WaitForSingleObject(g_mux, INFINITE);
        // 임계영역 시작!!!!!!!

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
                // 100점마다 tang 증가 로직, 최대 8개
                if ((score % 100 == 0) && (tangs.size() < 8)) {
                    // tang 증가
                    Player newTang;
                    /*
                        랜덤 위치 생성 로직
                        1. rand() 함수를 통해서 램덤 위치 생성, 이때 전체 맵 크기와 tang의 넓이를 계산하여 랜덤 값 생성
                        2. do while()를 이용하여 위치 생성 이후, 유저와 겹쳐서 생성 시, 다시 위치 생성
                        3. 조건 만족 시, 여러 tang들을 관리하기 위한 tangs(vector)에 push_back
                    */
                    do {
                        newTang.setObj(rand() % (map.right - newTang.getWidth() - map.left) + map.left, rand() % (map.bottom - newTang.getHeight() - map.top) - map.top);
                    } while (IntersectRect(&re, user.getObj(), newTang.getObj()));
                    tangs.push_back(newTang);
                }
                // 100점마다 food 증가 로직, 최대 15개
                if ((score % 100 == 0) && (foods.size() < 15)) {
                    // food 증가
                    Player newFood;
                    newFood.setWidth(30);
                    newFood.setHeight(30);
                    do {
                        newFood.setObj(rand() % (map.right - newFood.getWidth() - map.left) + map.left, rand() % (map.bottom - newFood.getHeight() - map.top) + map.top);
                    } while (IntersectRect(&re, user.getObj(), newFood.getObj()));
                    foods.push_back(newFood);
                }
            }
        }

        // 임계영역 해제!!!!!!!
        ReleaseMutex(g_mux);
    }
    ExitThread(0);
    return 0;
}

// tangs 스레드
DWORD WINAPI TangsLogic(LPVOID param) {
    HWND hWnd = (HWND)param;
    while (isGamePlaying && !isGameOver) {
        Sleep(16);
        WaitForSingleObject(g_mux, INFINITE);
        // 임계영역 시작!!!!!!!

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
                    // ex) dirX = a.left - b.left => dirX는 b가 a로 향하는 x축 방향 벡터 값★★★★ 
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
            // ex) dirX = a.left - b.left => dirX는 b가 a로 향하는 x축 방향 벡터 값★★★★
            float dirX = user.getLeft() - tangs[i].getLeft();
            float dirY = user.getTop() - tangs[i].getTop();

            // 계산한 방향 벡터를 정규화
            normalization(&dirX, &dirY);

            // tang에게 정규화된 방향으로 힘을 가함
            int increasedTime = score / TANG_UNIT_SCORE;
            float tangForce = TANG_DEFAULT_CHASE_FORCE + (TANG_ADDITIONAL_CHASE_FORCE * increasedTime);
            // tang의 최대 속도 =>  user의 70%
            if (tangForce > USER_FORCE * 0.6) {
                tangForce = USER_FORCE * 0.6;
            }
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

        bool isCrashed = false;
        // user와 tang들 충돌 검사 ()
        for (int i = 0; i < tangs.size(); i++) {
            // user 와 tang의 충돌 확인 및 타이머 해제
            RECT ret_tang;
            if (IntersectRect(&ret_tang, user.getObj(), tangs[i].getObj())) {
                isCrashed = true;
                break;
            }
        }

        // 임계영역 해제!!!!!!!
        ReleaseMutex(g_mux);

        if (isCrashed) {
            KillTimer(hWnd, FRAME_60FPS);
            KillTimer(hWnd, ONE_SECOND);
            changeGameStatus(GAME_OVER);
            MessageBox(hWnd, L"Game Over", L"Info", MB_OK);
            changeGameStatus(INIT_USER_ARROW_KEY);
            InvalidateRect(hWnd, NULL, FALSE);
        }
    }
    ExitThread(0);
    return 0;
}

// bullet 스레드
DWORD WINAPI BulletsLogic(LPVOID param) {
    HWND hWnd = (HWND)param;
    int lastShotTime = -1;
    while (isGamePlaying && !isGameOver) {
        Sleep(16);
        WaitForSingleObject(g_mux, INFINITE);
        // 임계영역 시작!!!!!!!
        int creation_time = BULLET_DEFAULT_TIME - (score / BULLET_UNIT_SCORE) * BULLET_REDUCED_TIME;
        if (creation_time < BULLET_TIME_MIN) {
            creation_time = BULLET_TIME_MIN;
        }
        if (gameTime > 0 && (gameTime % creation_time == 0) && (gameTime != lastShotTime)) {
            lastShotTime = gameTime;

            Player newBullet;

            newBullet.setWidth(BULLET_SIZE);
            newBullet.setHeight(BULLET_SIZE);

            // 맵 가장자리 랜덤 위치 선정
            // 0:상, 1:하, 2:좌, 3:우
            int edge = rand() % 4;
            int startX = 0, startY = 0;

            switch (edge) {
            case 0: // 상 (Top)
                startX = rand() % (map.right - map.left) + map.left;
                startY = map.top - BULLET_SIZE;
                break;
            case 1: // 하 (Bottom)
                startX = rand() % (map.right - map.left) + map.left;
                startY = map.bottom;
                break;
            case 2: // 좌 (Left)
                startX = map.left - BULLET_SIZE;
                startY = rand() % (map.bottom - map.top) + map.top;
                break;
            case 3: // 우 (Right)
                startX = map.right;
                startY = rand() % (map.bottom - map.top) + map.top;
                break;
            }
            newBullet.setObj(startX, startY);

            // 생성되는 순간 유저를 향해 방향 계산 (직선)
            float dirX = user.getLeft() - startX;
            float dirY = user.getTop() - startY;
            normalization(&dirX, &dirY);

            // 속도 적용 (한 번 정해지면 바뀌지 않음)
            newBullet.setVx(dirX * BULLET_SPEED);
            newBullet.SetVy(dirY * BULLET_SPEED);

            bullets.push_back(newBullet);
        }

        auto it = bullets.begin();
        while (it != bullets.end()) {

            // constantMove가 false를 반환하면(맵 밖으로 나감) -> 삭제
            // constantMove가 true를 반환하면(맵 안임) -> 살려둠
            if (it->constantMove(map) == false) {
                it = bullets.erase(it); // 삭제하고 다음 요소 받기
            }
            else {
                ++it; // 다음 요소로 이동
            }
        }

        // 3. 충돌 검사 (User vs Bullets)
        bool isCrashed = false;
        for (int i = 0; i < bullets.size(); i++) {
            RECT ret;
            if (IntersectRect(&ret, user.getObj(), bullets[i].getObj())) {
                isCrashed = true;
                break;
            }
        }
        // 임계영역 해제!!!!!!!
        ReleaseMutex(g_mux);

        if (isCrashed) {
            KillTimer(hWnd, FRAME_60FPS);
            KillTimer(hWnd, ONE_SECOND);
            changeGameStatus(GAME_OVER);
            MessageBox(hWnd, L"Game Over", L"Info", MB_OK);
            changeGameStatus(INIT_USER_ARROW_KEY);
            InvalidateRect(hWnd, NULL, FALSE);
        }
    }
    ExitThread(0);
    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        // mutex 초기화
        g_mux = CreateMutex(NULL, FALSE, L"mutex");
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
                /// Mutex 스레드 생성
                if (hUserThread != NULL) {
                    CloseHandle(hUserThread);
                }
                if (hTangThread != NULL) {
                    CloseHandle(hTangThread);
                }
                if (hBulletThread != NULL) {
                    CloseHandle(hBulletThread);
                }
                hUserThread = CreateThread(NULL, 0, UserLogic, hWnd, 0, NULL);
                hTangThread = CreateThread(NULL, 0, TangsLogic, hWnd, 0, NULL);
                hBulletThread = CreateThread(NULL, 0, BulletsLogic, hWnd, 0, NULL);
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
            if (isGameOver) {
                changeGameStatus(MAIN_MENU);
                InvalidateRect(hWnd, NULL, FALSE);
            }
        }
        break;
        case VK_F2:
        {
            if (isMainMenu) {
                changeGameStatus(DEBUG_MODE);
                InvalidateRect(hWnd, NULL, FALSE);
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

        WaitForSingleObject(g_mux, INFINITE);
        // 임계영역 시작!!!!!!!

        // hdc 에는 그림을 그릴 대상과 연결되어있다.
        // (설정된 값과 도구의 모음), (그려진 그림의 데이터는 가지고 있지 않는다.)

        // 더블 버퍼링 구현(중요)
        // GDI 객체 선언
        HDC hMemDC;
        HBITMAP hBitmap;
        HBITMAP hOldBitmap;
        HPEN noPen = CreatePen(PS_NULL, NULL, NULL);
        HPEN oldPen;
        // 브러쉬
        HBRUSH userBrush = CreateSolidBrush(RGB(0, 0, 0));
        HBRUSH tangBrush = CreateSolidBrush(RGB(255, 0, 0));
        HBRUSH holdBrush = CreateSolidBrush(RGB(0, 20, 150));
        HBRUSH foodBrush = CreateSolidBrush(RGB(0, 150, 0));
        HBRUSH bulletBrush = CreateSolidBrush(RGB(255, 212, 0));
        HBRUSH oldBrush;
        // 폰트
        HFONT titleFont = CreateFont(50, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
            DEFAULT_PITCH | FF_SWISS, L"Arial");
        HFONT startFont = CreateFont(30, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
            DEFAULT_PITCH | FF_SWISS, L"Arial");
        HFONT detailFont = CreateFont(30, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
            DEFAULT_PITCH | FF_SWISS, L"Arial");
        HFONT debugFont = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
            DEFAULT_PITCH | FF_SWISS, L"Arial");
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
            oldFont = (HFONT)SelectObject(hMemDC, titleFont);
            // 게임 타이틀
            WCHAR gameMainTitle[30] = L"Bullet-Heaven";
            TextOut(hMemDC, map.left + ((map.right - map.left) / 2), map.top + 100, gameMainTitle, lstrlen(gameMainTitle));
            SelectObject(hMemDC, oldFont);

            // 게임 시작 버튼 && start 텍스트
            SelectObject(hMemDC, startFont);
            Rectangle(hMemDC, startButton.left, startButton.top, startButton.right, startButton.bottom);
            WCHAR startButtonText[10] = L"STRAT";
            TextOut(hMemDC, startButton.left + ((startButton.right - startButton.left) / 2), startButton.top + 10, startButtonText, lstrlen(startButtonText));
            SelectObject(hMemDC, oldFont);
            SetTextAlign(hMemDC, TA_LEFT);

            if (isDebugMode) {
                oldFont = (HFONT)SelectObject(hMemDC, detailFont);
                WCHAR d_mode[20] = L"debug mode ON";
                TextOut(hMemDC, map.right + 10, map.top + 10, d_mode, lstrlen(d_mode));
                SelectObject(hMemDC, oldFont);
            }
        }

        // 게임 상태가 진행 중 일때, 그리는 부분
        if (isGamePlaying) {
            int text_start = 10;
            int textV_gap = 30;

            oldFont = (HFONT)SelectObject(hMemDC, detailFont);
            // 게임 타이머
            WCHAR timerText[100];
            wsprintfW(timerText, L"Time : %d", gameTime);
            TextOut(hMemDC, map.right + 10, map.top + text_start, timerText, lstrlenW(timerText));
            text_start += textV_gap;

            // 게임 스코어
            WCHAR scoreText[100];
            wsprintfW(scoreText, L"Score : %d", score);
            TextOut(hMemDC, map.right + 10, map.top + text_start, scoreText, lstrlenW(scoreText));
            text_start += textV_gap;

            // 대쉬
            WCHAR dashText[50];
            if (dashCount == 0) {
                wsprintfW(dashText, L"Dash Ready!");
            }
            else {
                wsprintfW(dashText, L"Dash CoolTime : %d", dashCount);
            }
            TextOut(hMemDC, map.right + 10, map.top + text_start, dashText, lstrlenW(dashText));
            text_start += textV_gap;

            SelectObject(hMemDC, oldFont);

            // 디버그 모드 활성화 시, 출력 되는 부분
            if (isDebugMode) {
                oldFont = (HFONT)SelectObject(hMemDC, debugFont);
                int debug_text_start = text_start + 100;

                // tang, food, bullet의 개수
                WCHAR objectCount[100];
                wsprintfW(objectCount, L"tang의 수 : %d, food의 수 : %d, bullet의 수 : %d", (int)tangs.size(), (int)foods.size(), (int)bullets.size());
                TextOut(hMemDC, map.right + 10, map.top + debug_text_start, objectCount, lstrlenW(objectCount));
                debug_text_start += textV_gap;

                // user의 속도
                WCHAR userSpeed[50];
                swprintf_s(userSpeed, L"user의 속도 => vx : %.2f, vy : %.2f", user.getVx(), user.getVy());
                TextOut(hMemDC, map.right + 10, map.top + debug_text_start, userSpeed, lstrlenW(userSpeed));
                debug_text_start += textV_gap;

                // tang들의 속도
                WCHAR tangSpeed[50];
                for (int i = 0; i < tangs.size(); i++) {
                    memset(tangSpeed, 0x00, 50);
                    swprintf_s(tangSpeed, L"tang[%d]의 속도 => vx : %.2f, vy : %.2f", i, tangs[i].getVx(), tangs[i].getVy());
                    TextOut(hMemDC, map.right + 10, map.top + debug_text_start, tangSpeed, lstrlenW(tangSpeed));
                    debug_text_start += textV_gap;
                }
                SelectObject(hMemDC, oldFont);
            }

            // user 위치
            oldBrush = (HBRUSH)SelectObject(hMemDC, userBrush);
            Rectangle(hMemDC, user.getLeft(), user.getTop(), user.getRight(), user.getBottom());
            SelectObject(hMemDC, oldBrush);
            
            // bullets 위치
            for (int i = 0; i < bullets.size(); i++) {
                oldBrush = (HBRUSH)SelectObject(hMemDC, bulletBrush);
                Ellipse(hMemDC, bullets[i].getLeft(), bullets[i].getTop(), bullets[i].getRight(), bullets[i].getBottom());
                SelectObject(hMemDC, oldBrush);
            }

            // tangs 위치
            for (int i = 0; i < tangs.size(); i++) {
                if (!tangs[i].getIsHold()) {
                    oldBrush = (HBRUSH)SelectObject(hMemDC, tangBrush);
                    Ellipse(hMemDC, tangs[i].getLeft(), tangs[i].getTop(), tangs[i].getRight(), tangs[i].getBottom());
                    SelectObject(hMemDC, oldBrush);
                }
                else {
                    oldBrush = (HBRUSH)SelectObject(hMemDC, holdBrush);
                    Ellipse(hMemDC, tangs[i].getLeft(), tangs[i].getTop(), tangs[i].getRight(), tangs[i].getBottom());
                    SelectObject(hMemDC, oldBrush);
                }
            }

            // foods 위치
            oldBrush = (HBRUSH)SelectObject(hMemDC, foodBrush);
            for (int i = 0; i < foods.size(); i++) {
                Ellipse(hMemDC, foods[i].getLeft(), foods[i].getTop(), foods[i].getRight(), foods[i].getBottom());
            }
            SelectObject(hMemDC, oldBrush);
            
            // enemy 게이지 바
            Rectangle(hMemDC, map.left, map.bottom, map.right, map.bottom + 10); // 전체 게이지 바

            oldBrush = (HBRUSH)SelectObject(hMemDC, tangBrush);
            int enemyGauge = map.left;
            // 게이지 전체는 100, 게이지 간격 20, 20당 구간 길이 => (map.right - map.left) / 5
            enemyGauge += (score % 100 / 20) * (map.right - map.left) / 5;
            Rectangle(hMemDC, map.left, map.bottom, enemyGauge, map.bottom + 10);

            SelectObject(hMemDC, oldBrush);

            // 게이지 간격
            for (int i = 1; i <= 4; i++) {
                int gaugeGap = (map.right - map.left) / 5;
                MoveToEx(hMemDC, map.left + gaugeGap * i, map.bottom, NULL);
                LineTo(hMemDC, map.left + gaugeGap * i, map.bottom + 10);
            }
            

            // 게임 상태가 게임 오버 일때, 그리는 부분
            if (isGameOver) {
                oldFont = (HFONT)SelectObject(hMemDC, detailFont);
                // f1키 입력 안내
                WCHAR overText[50] = L"F1키 입력 시, 메인 메뉴 이동";
                TextOut(hMemDC, map.right + 10, map.top + text_start, overText, lstrlen(overText));
                text_start += textV_gap;

                SelectObject(hMemDC, oldFont);
            }
        }

        // 백버퍼에 그려진 내용을 hdc로 복사
        BitBlt(hdc, 0, 0, rect.right, rect.bottom, hMemDC, 0, 0, SRCCOPY);

        // 리소스 해제
        SelectObject(hMemDC, hOldBitmap);
        DeleteObject(noPen);
        DeleteObject(userBrush);
        DeleteObject(tangBrush);
        DeleteObject(holdBrush);
        DeleteObject(foodBrush);
        DeleteObject(titleFont);
        DeleteObject(startFont);
        DeleteObject(detailFont);
        DeleteObject(debugFont);
        DeleteObject(hBitmap);
        DeleteObject(hMemDC);

        // 임계영역 해제!!!!!!!
        ReleaseMutex(g_mux);

        // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
    {
        /// Mutex 해제
        CloseHandle(g_mux);
        if (hUserThread != NULL) {
            CloseHandle(hUserThread);
        }
        if (hTangThread != NULL) {
            CloseHandle(hTangThread);
        }
        if (hBulletThread != NULL) {
            CloseHandle(hBulletThread);
        }
        //DestroyWindow(g_button);
        tangs.clear();
        foods.clear();
        bullets.clear();
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
    bullets.clear();
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
        isGameOver = true;
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
    case DEBUG_MODE: 
    {
        isDebugMode = !isDebugMode;
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