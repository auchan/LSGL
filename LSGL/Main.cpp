// LSGL.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "LSGL.h"
#include <cstdio>
#include <vector>
#include "Application.h"
using namespace lsgl;

#define printf printf2
int __cdecl printf2(const char *format, ...)
{
	char str[1024];

	va_list argptr;
	va_start(argptr, format);
	int ret = vsnprintf(str, sizeof(str), format, argptr);
	va_end(argptr);

	OutputDebugStringA(str);

	return ret;
}

#define MAX_LOADSTRING 100

// Global Variables:
HWND GMainWindowHandle = nullptr; // globally track main window
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
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

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LSGL, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LSGL));

    MSG msg;

	HWND hwnd = GMainWindowHandle;
	// get the dc and hold it
	HDC hdc = GetDC(hwnd);

	int viewWidth = 640;
	int viewHeight = 512;
	HBITMAP hBitmap = CreateCompatibleBitmap(hdc, viewWidth, viewHeight);
	HDC hMemDC = CreateCompatibleDC(hdc);
	SelectObject(hMemDC, hBitmap);

	BITMAPINFO bmpInfo;
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = viewWidth;
	// pay attention to the sign, you most likely want a
	// top-down pixel array as it's easier to use
	bmpInfo.bmiHeader.biHeight = viewHeight;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 24;
	bmpInfo.bmiHeader.biCompression = BI_RGB;
	bmpInfo.bmiHeader.biSizeImage = 0;
	bmpInfo.bmiHeader.biClrUsed = 0;
	bmpInfo.bmiHeader.biClrImportant = 0;

	long nLnBytes = bmpInfo.bmiHeader.biWidth * 3;
	BYTE *pData = new BYTE[nLnBytes * abs(bmpInfo.bmiHeader.biHeight)];
	//清成黑色
	memset(pData, 0, nLnBytes * abs(bmpInfo.bmiHeader.biHeight));
	Application &app = Application::instance();
	app.init(viewWidth, viewHeight);
    // Main message loop:
    while (true)
    {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		// main game processing goes here

		// set the foreground color to random
		//SetTextColor(hdc, RGB(rand() % 256, rand() % 256, rand() % 256));

		//// set the background color to black
		SetBkColor(hdc, RGB(0, 0, 0));

		//// finally set the transparency mode to transparent
		//SetBkMode(hdc, TRANSPARENT);

		// draw some text at a random location
		//TextOut(hdc, rand() % 800, rand() % 800, L"Green Light", strlen("Green Light"));
		//SetPixel(hdc, rand() % 800, rand() % 800, RGB(rand() % 256, rand() % 256, rand() % 256));
		//for (int i = 0; i < viewWidth / 2; i++)
		//{
		//	for (int j = 0; j < viewHeight / 2; j++)
		//	{
		//		pData[j * nLnBytes + i * 3 + 0] = rand() % 256;
		//		pData[j * nLnBytes + i * 3 + 1] = rand() % 256;
		//		pData[j * nLnBytes + i * 3 + 2] = rand() % 256;
		//	}
		//}

		app.update();
		RenderBuffer *renderBuffer = app.getColorBuffer();
		if (renderBuffer)
		{
			SetDIBits(hMemDC, hBitmap, 0, abs(bmpInfo.bmiHeader.biHeight), renderBuffer->getData(), &bmpInfo, DIB_RGB_COLORS);
			BitBlt(hdc, 0, 0, bmpInfo.bmiHeader.biWidth, abs(bmpInfo.bmiHeader.biHeight), hMemDC, 0, 0, SRCCOPY);
		}
		app.postUpdate();
    }
	Application::instance().quit();

	delete[] pData;
	DeleteObject(SelectObject(hMemDC, hBitmap));
	DeleteDC(hMemDC);

	// release the dc
	ReleaseDC(hwnd, hdc);

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LSGL));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LSGL);
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

   GMainWindowHandle = hWnd;

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
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
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
	case WM_KEYUP:
		Application::instance().onKeyUp(static_cast<int>(wParam));
		return DefWindowProc(hWnd, message, wParam, lParam);
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{	
			PostQuitMessage(0);
			return 0;
		}
		Application::instance().onKeyDown(static_cast<int>(wParam));
		return DefWindowProc(hWnd, message, wParam, lParam);
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
