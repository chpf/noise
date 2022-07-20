#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <cstdint>
#include <comdef.h>
#include <string>

#include "noise.hpp"

static bool running = true;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

LPCWSTR fromString(std::string s)
{
	std::wstring stemp = std::wstring(s.begin(), s.end());
	LPCWSTR sw = stemp.c_str();
	return sw;
}

struct Debug_help {
	bool q = false;
	bool m = false;
	int pox = 0;
	int poy = 0;
};

static Debug_help db = {};

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		    PWSTR pCmdLine, int nCmdShow)
{
	OutputDebugString(L"Start\n");

	//PlaySound(TEXT("taps.wav"), NULL, SND_FILENAME);

	// Register the window class.
	const wchar_t CLASS_NAME[] = L"Noise Window Class";
	WNDCLASS wc = {};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	wc.hCursor = LoadCursor(NULL, IDC_CROSS); // class cursor

	RegisterClass(&wc);

	HWND hwnd = CreateWindowEx(
		NULL, // Optional window styles.
		CLASS_NAME, // Window class
		L"Noise Generator", // Window text
		WS_SYSMENU | WS_CAPTION |
			WS_MINIMIZEBOX, // no fullscreen, no resizedwStyle,            // Window style
		//x,           y,             width, height,
		CW_USEDEFAULT, CW_USEDEFAULT, 256 * 2, 192 * 2,
		NULL, // Parent window
		NULL, // Menu
		hInstance, // Instance handle
		NULL // Additional application data
	);

	if (hwnd == NULL) {
		// could not create window
		return -1;
	}

	ShowWindow(hwnd, nCmdShow);

	Noise noise = {};
	HRESULT hr = {};
	if (S_OK != noise.init(hwnd)) {
		_com_error err(hr);
		LPCTSTR errMsg = err.ErrorMessage();
		OutputDebugString(errMsg);
		return -1;
	}
	noise.play();

	MSG msg = {};
	POINT cursor_pos = {};
	while (running) {
		while (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		GetCursorPos(&cursor_pos);
		ScreenToClient(
			hwnd,
			&cursor_pos); // can be negative when outside of window

		db.pox = cursor_pos.x;
		db.poy = cursor_pos.y;

		//noise.play();

		if (db.q)
			running = false;
		if (db.m)
			noise.pause();

		//timing
	}
	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = {};
	switch (uMsg) {
	case WM_CHAR: {
		if ((wchar_t)wParam == 'q') {
			db.q = true;
		}
		if ((wchar_t)wParam == 'm') {
			db.m = true;
		}
	} break;
	case WM_SIZING:
	case WM_SIZE: {
		// ignore resize
	} break;
	case WM_CLOSE:
	case WM_DESTROY: {
		PostQuitMessage(0);
		running = false;
	} break;
	case WM_PAINT: {
		OutputDebugString(L"WM_PAINT\n");
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		SetTextColor(hdc, RGB(18, 18, 18));
		RECT rec;
		SetRect(&rec, 10, 10, 256, 192);
		auto x = std::to_string(db.pox);
		auto y = std::to_string(db.poy);
		DrawText(hdc, fromString(std::string("AAAAAAA")), -1, &rec,
			 DT_TOP | DT_CENTER);
		SetTextColor(hdc, RGB(18, 180, 18));
		DrawText(hdc, fromString(x), -1, &rec, DT_CENTER | DT_CENTER);
		SetTextColor(hdc, RGB(18, 18, 180));
		DrawText(hdc, fromString(y), -1, &rec, DT_BOTTOM | DT_CENTER);
		EndPaint(hwnd, &ps);
		ReleaseDC(hwnd, hdc);
	} break;
	default: {
		result = DefWindowProc(hwnd, uMsg, wParam, lParam);
	} break;
	}
	return result;
}