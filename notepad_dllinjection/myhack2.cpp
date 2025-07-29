// myhack.cpp (멀티바이트 빌드 기준 예시)

#include <windows.h>
#include <shellapi.h>          // ShellExecuteA 선언
#include <tchar.h>

// 브라우저를 띄우는 URL
#define DEF_URL  ("https://www.naver.com")

#pragma comment(lib, "shell32.lib")   // ↓ urlmon.lib 대신 shell32.lib

HMODULE g_hMod = NULL;

// ──────────────────────────────────────────────────────────────
// ① 더 이상 파일 경로 계산이 필요 없으므로 코드를 완전히 단순화
//    → 스레드가 돌면 바로 기본 브라우저로 네이버 오픈
DWORD WINAPI ThreadProc(LPVOID /*lParam*/)
{
    HINSTANCE hInst = ShellExecuteA(
        NULL,                  // hwnd
        "open",                // lpOperation
        DEF_URL,               // lpFile (URL)
        NULL, NULL,
        SW_SHOWNORMAL);        // 창 모드

    // 성공이면 32 이상, 실패면 32 미만 오류코드가 반환
    if ((INT_PTR)hInst <= 32)
        OutputDebugStringA("ShellExecuteA failed!");

    return 0;
}
// ──────────────────────────────────────────────────────────────

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID)
{
    g_hMod = (HMODULE)hinstDLL;

    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        OutputDebugStringA("myhack.dll injected!");
        {
            HANDLE hThread = CreateThread(
                NULL, 0, ThreadProc, NULL, 0, NULL);
            if (hThread) CloseHandle(hThread);
        }
        break;
    }
    return TRUE;
}
