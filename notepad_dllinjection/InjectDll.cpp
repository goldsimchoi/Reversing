// InjectDll.cpp
#include <stdio.h>
#include "windows.h"
#include "tchar.h"

BOOL InjectDll(DWORD dwPID, LPCTSTR szDllPath)
{
    HANDLE hProcess = NULL, hThread = NULL;
    HMODULE hMod = NULL;
    LPVOID pRemoteBuf;
    DWORD dwBufSize = (DWORD)(strlen(szDllPath) + 1);            // strlen/char
    //DWORD dwBufSize = (DWORD)(_tcslen(szDllPath) + 1) * sizeof(TCHAR);
    LPTHREAD_START_ROUTINE pThreadProc;

    // #1. dwPID를 이용하여 대상 프로세스(notepad.exe)의 HANDLE을 구한다.
    if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID)))
    {
        _tprintf("OpenProcess(%d) failed!!! [%d]\n", dwPID,
            GetLastError());
        return FALSE;
    }

    // #2. 대상 프로세스(notepad.exe) 메모리에 szDllName 크기만큼 메모리를 할당한다.
    pRemoteBuf = VirtualAllocEx(hProcess, nullptr, dwBufSize, MEM_COMMIT, PAGE_READWRITE);
    WriteProcessMemory(hProcess, pRemoteBuf, szDllPath, dwBufSize, nullptr);


    //pRemoteBuf = VirtualAllocEx(hProcess, NULL, dwBufSize, MEM_COMMIT,
    //    PAGE_READWRITE);

    // #3. 할당 받은 메모리에 myhack.dll 경로("c:\\myhack.dll")을 쓴다.
    WriteProcessMemory(hProcess, pRemoteBuf, (LPVOID)szDllPath, dwBufSize,
        NULL);

    // #4. LoadLibraryW API 주소를 구한다.
    hMod = GetModuleHandle("kernel32.dll");

    //ANSI로 통일해봄
    pThreadProc =
        (LPTHREAD_START_ROUTINE)GetProcAddress(
            GetModuleHandle("kernel32.dll"), "LoadLibraryA");

    /*이슈로인해..
    pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hMod,
                                                        "LoadLibraryW");
                                                        */


                                                        // #5. notepad.exe 프로세스에 스레드를 실행
    hThread = CreateRemoteThread(hProcess, NULL,           // hProcess
        // lpThreadAttributes
        0,                       // dwStackSize
        pThreadProc,             // lpStartAddress
        pRemoteBuf,              // lpParameter
        0,                       // dwCreationFlags
        NULL);                   // lpThreadId

    WaitForSingleObject(hThread, INFINITE);

    CloseHandle(hThread);           // strlen/char

    CloseHandle(hProcess);

    return TRUE;
}

int _tmain(int argc, TCHAR* argv[])
{
    if (argc != 3)
    {
        _tprintf("USAGE : %s pid DLL_path\n", argv[0]);
        return 1;
    }

    // inject dll
    if (InjectDll((DWORD)_tstoi(argv[1]), argv[2]))
        _tprintf("InjectDll(\"%s\") success!!\n", argv[2]);
    else
        _tprintf("InjectDll(\"%s\") failed!!!\n", argv[2]);

    return 0;
}
