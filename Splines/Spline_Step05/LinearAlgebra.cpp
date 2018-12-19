// LinearAlgebra.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "LinearAlgebra.h"
//_20180519_jintaeks
#include "KVectorUtil.h"
#include <MMSystem.h>
#include "KMatrix3.h"
#include <objidl.h>
#include <gdiplus.h>
#include <windowsx.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include "KHermiteSpline2.h"

using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")
#pragma comment(lib,"winmm.lib")

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
// _20180519_jintaeks
HWND        g_hwnd = NULL;
HDC         g_hdc = 0;
HBITMAP     g_hBitmap = 0;
RECT        g_clientRect;
UINT        g_mouseLButtonDown = 0;
KVector2    g_vLineEnd{ 0, 0 };

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void                Initialize();
void                Finalize();
void                OnSize();
void                OnIdle(float fElapsedTime_);
void                OnLButtonDown(LONG x, LONG y);
void                OnLButtonUp(LONG x, LONG y);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR           gdiplusToken;

    // Initialize GDI+.
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LINEARALGEBRA, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LINEARALGEBRA));

    Initialize();

    DWORD dwOldTime = ::timeGetTime();

    MSG msg;

    // Main message loop:
    while (true)
    {
        ::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
        const DWORD dwNewTime = ::timeGetTime();
        const BOOL bIsTranslateMessage = TranslateAccelerator(msg.hwnd, hAccelTable, &msg);
        if (!bIsTranslateMessage)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }//if

        OnIdle(float(dwNewTime - dwOldTime) / 1000.f);
        Sleep(10);

        dwOldTime = dwNewTime;

        if (msg.message == WM_QUIT)
        {
            break;
        }//if
    }//while

    Finalize();
    GdiplusShutdown(gdiplusToken);

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LINEARALGEBRA));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LINEARALGEBRA);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   // _20180519_jintaeks
   g_hwnd = hWnd;
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


//_20180519_jintaeks
void OnPaint(HDC hdc)
{
    //KVectorUtil::DrawLine(hdc, KVector2(0, 0), KVector2(3, 2), 1, PS_DASH, RGB(0, 0, 255));
    //KVectorUtil::DrawLine(hdc, KVector2(0, 0), KVector2(2, 3), 2, PS_SOLID, RGB(255, 0, 0));
    Graphics graphics(hdc);
    Pen      pen(Color(255, 0, 0, 255));
    graphics.DrawLine(&pen, 0, 0, 200, 100);
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
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
            // TODO: Add any drawing code that uses hdc here...
            // _20180519_jintaeks
            OnPaint( hdc );
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_SIZE:
        OnSize();
        break;
    case WM_LBUTTONDOWN:
        OnLButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;
    case WM_LBUTTONUP:
        OnLButtonUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
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

void Initialize()
{
}//Initialize()

void Finalize()
{
    if (g_hdc != 0) {
        DeleteDC(g_hdc);
        g_hdc = 0;
    }//if
    if (g_hBitmap != 0) {
        DeleteObject(g_hBitmap);
        g_hBitmap = 0;
    }//if
}//Finalize()

void OnSize()
{
    Finalize();

    ::GetClientRect(g_hwnd, &g_clientRect);
    const int iWidth = g_clientRect.right - g_clientRect.left + 1;
    const int iHeight = g_clientRect.bottom - g_clientRect.top + 1;

    KVector2 origin;
    origin.x = iWidth / 2.0f;
    origin.y = iHeight / 2.0f;
    KVectorUtil::g_screenCoordinate.SetInfo(KVector2(10, 0), KVector2(0, -10), origin);

    HDC hdc = ::GetDC(g_hwnd);
    g_hdc = CreateCompatibleDC(hdc);
    g_hBitmap = CreateCompatibleBitmap(hdc, iWidth, iHeight);
    SelectObject(g_hdc, g_hBitmap);
}//OnSize()

float Lerp(const float begin, const float end, float ratio_)
{
    const float ratio = __min(1, __max(0, ratio_));
    float temp;
    temp = begin + (end - begin) * ratio;
    return temp;
}

void OnIdle(float fElapsedTime_)
{
    const int iWidth = g_clientRect.right - g_clientRect.left + 1;
    const int iHeight = g_clientRect.bottom - g_clientRect.top + 1;

    HDC hdc = ::GetDC(g_hwnd);

    HBRUSH brush;
    brush = CreateSolidBrush(RGB(255, 255, 255));
    HGDIOBJ oldBrush = SelectObject(g_hdc, brush);
    Rectangle(g_hdc, 0, 0, iWidth, iHeight);

    // draw grid and axis
    {
        KBasis2     basis2;
        basis2.SetInfo(KVector2(1, 0), KVector2(0, 1));
        KVectorUtil::SetBasis2(basis2);

        //KVectorUtil::DrawGrid(g_hdc, 100, 100);
        KVectorUtil::DrawAxis(g_hdc, 100, 100, RGB(255,0,0), RGB(255,0,0));
    }

    // draw a Hermite spline
    {
        KHermiteSpline2     spline;
        KVector2 p1( 0, 0 );
        KVector2 p2( 30, -10 );
        KVector2 r1 = g_vLineEnd*10.0f;
        KVector2 r2( 100, 100 );
        spline.Construct( p1, p2, r1, r2 );

        KVector2 prev;
        KVector2 curr;
        float t = 0.f;
        const float fStep = 0.05f;
        const int steps = int( 1.f / fStep );
        prev = spline.GetPosition( 0 );
        for( int i = 0; i <= steps; ++i, t += fStep )
        {
            curr = spline.GetPosition( t );
            KVectorUtil::DrawLine( g_hdc, prev, curr, 2 );
            prev = curr;
        }//for

        KVector2 q1 = p1 + r1 * 0.25;
        KVector2 q2 = p2 + r2 * 0.25;
        KVectorUtil::DrawLine( g_hdc, p1, p1 + r1 * 0.1, 1, PS_DASHDOT, RGB( 255, 0, 0 ) );
        KVectorUtil::DrawLine( g_hdc, p1, p1 - r1 * 0.1, 1, PS_DASHDOT, RGB( 255, 0, 0 ) );
        KVectorUtil::DrawLine( g_hdc, p2, p2 + r2 * 0.1, 1, PS_DASHDOT, RGB( 255, 0, 0 ) );
        KVectorUtil::DrawLine( g_hdc, p2, p2 - r2 * 0.1, 1, PS_DASHDOT, RGB( 255, 0, 0 ) );

        //const float u = 0.5f;
        //KVector2 position = spline.GetPosition(u);
        //KVector2 tangent = spline.GetTangent(u);
        //KVectorUtil::DrawLine(g_hdc, position, position + tangent * 100.f);
    }

    BitBlt(hdc, 0, 0, iWidth, iHeight, g_hdc, 0, 0, SRCCOPY);
    SelectObject(g_hdc, oldBrush);
    DeleteObject(brush);

    ::ReleaseDC(g_hwnd, hdc);
}//OnIdle()

void OnLButtonDown( LONG x, LONG y )
{
    if (g_mouseLButtonDown != 1) {
        POINT mousePoint;
        GetCursorPos(&mousePoint);
        ScreenToClient(g_hwnd, &mousePoint);
        KVector2 vmouse = KVectorUtil::ScreenToWorld(KVector2(mousePoint.x, mousePoint.y));
        g_vLineEnd = vmouse;
    }
    g_mouseLButtonDown = 1;
}

void OnLButtonUp( LONG x, LONG y )
{
    g_mouseLButtonDown = 0;
}
