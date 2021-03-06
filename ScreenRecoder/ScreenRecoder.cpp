//ScreenRecoder.cpp: 定义应用程序的入口点。
//

#include "stdafx.h"
#include "ScreenRecoder.h"
#include "resource.h"
#include "App.h"
#include <Shlwapi.h> 
#include <shellapi.h>
#include "StringHlp.h"

#pragma comment(lib,"DbgHelp.Lib") 
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "winmm.lib")

//
// 应用底层承载入口
//
// 此应用使用 Mono 来运行 C# 程序，因此脱离了.net。
// 应用入口
// 
// 单文件
// 2017.8.26 by DreamFish
// 
// 优化
// 2019.8.29 by 梦欤
//

using namespace std;

LPWSTR *szArgList;
int argCount;
HINSTANCE hInst;

App*app = nullptr;
HANDLE hMutex = nullptr;

int main() 
{
	hInst = GetModuleHandle(NULL);
	int rs = -1;

	setlocale(LC_ALL, "chs");

	//设置控制台窗口
	HWND hConsole = GetConsoleWindow();
	HICON hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_SCREENRECODER));
	SendMessage(hConsole, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	SendMessage(hConsole, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
	SetConsoleTitle(L"ScreenRecoder Debug Console");
	HMENU menu = GetSystemMenu(hConsole, FALSE);
	EnableMenuItem(menu, SC_CLOSE, MF_GRAYED | MF_DISABLED);

	//命令行参数处理
	szArgList = CommandLineToArgvW(GetCommandLine(), &argCount);
	if (szArgList == NULL)
	{
		MessageBox(NULL, L"Unable to parse command line", L"ScreenRecoder 错误", MB_OK | MB_ICONERROR);
		return rs;
	}

	app = new App();

	freopen_s(&app->fileIn, "CONOUT$", "w", stdin);
	freopen_s(&app->fileErr, "CONOUT$", "w", stderr);
	freopen_s(&app->fileOut, "CONOUT$", "w", stdout);


	//隐藏控制台
	if (!app->ShouldShowDebugWindow())
		ShowWindow(hConsole, SW_HIDE);

	app->InitIPath();

	//防止重复运行程序
	hMutex = CreateMutex(NULL, TRUE, L"ScreenRecoderApp");
	if (GetLastError() == ERROR_ALREADY_EXISTS && !app->HasArg(L"-ignore-multi-check"))
	{
		//有一个程序实例正在运行
		WCHAR buf[32];
		GetPrivateProfileStringW(L"AppSetting", L"LastWindow", L"0", buf, 32, app->iniPath);

		HWND hWnd = (HWND)_wtol(buf);
		if (IsWindow(hWnd)) {
			if (!IsWindowVisible(hWnd)) ShowWindow(hWnd, SW_SHOW);
			if (!IsIconic(hWnd)) ShowWindow(hWnd, SW_RESTORE);
			SetForegroundWindow(hWnd);
			rs = 0;
		}
		else {
			MessageBox(NULL, L"已经有一个程序实例正在运行，同时只能运行一个程序实例", L"ScreenRecoder 提示", MB_OK | MB_ICONERROR);
			rs = 0;
			goto QUIT;
		}
	}
	else {

		app->InitICrashFilter();

		if (!app->Load()) {
			MessageBox(NULL, L"初始化程序失败", L"ScreenRecoder 错误", MB_OK | MB_ICONERROR);
			goto QUIT;
		}

		app->InitICalls();
		rs = app->Run();
		app->Clear();
	}

QUIT:

	EnableMenuItem(menu, SC_CLOSE, MF_ENABLED);

	fclose(app->fileOut);
	fclose(app->fileErr);
	fclose(app->fileIn);

	delete app;

	if (hMutex) ReleaseMutex(hMutex);

	LocalFree(szArgList);
	return rs;
}

