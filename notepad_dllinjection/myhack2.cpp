// myhack.cpp (��Ƽ����Ʈ ���� ���� ����)

#include <windows.h>
#include <shellapi.h>          // ShellExecuteA ����
#include <tchar.h>

// �������� ���� URL
#define DEF_URL  ("https://www.naver.com")

#pragma comment(lib, "shell32.lib")   // �� urlmon.lib ��� shell32.lib

HMODULE g_hMod = NULL;

// ����������������������������������������������������������������������������������������������������������������������������
// �� �� �̻� ���� ��� ����� �ʿ� �����Ƿ� �ڵ带 ������ �ܼ�ȭ
//    �� �����尡 ���� �ٷ� �⺻ �������� ���̹� ����
DWORD WINAPI ThreadProc(LPVOID /*lParam*/)
{
    HINSTANCE hInst = ShellExecuteA(
        NULL,                  // hwnd
        "open",                // lpOperation
        DEF_URL,               // lpFile (URL)
        NULL, NULL,
        SW_SHOWNORMAL);        // â ���

    // �����̸� 32 �̻�, ���и� 32 �̸� �����ڵ尡 ��ȯ
    if ((INT_PTR)hInst <= 32)
        OutputDebugStringA("ShellExecuteA failed!");

    return 0;
}
// ����������������������������������������������������������������������������������������������������������������������������

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
