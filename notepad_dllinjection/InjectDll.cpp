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

    // #1. dwPID�� �̿��Ͽ� ��� ���μ���(notepad.exe)�� HANDLE�� ���Ѵ�.
    if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID)))
    {
        _tprintf("OpenProcess(%d) failed!!! [%d]\n", dwPID,
            GetLastError());
        return FALSE;
    }

    // #2. ��� ���μ���(notepad.exe) �޸𸮿� szDllName ũ�⸸ŭ �޸𸮸� �Ҵ��Ѵ�.
    pRemoteBuf = VirtualAllocEx(hProcess, nullptr, dwBufSize, MEM_COMMIT, PAGE_READWRITE);
    WriteProcessMemory(hProcess, pRemoteBuf, szDllPath, dwBufSize, nullptr);


    //pRemoteBuf = VirtualAllocEx(hProcess, NULL, dwBufSize, MEM_COMMIT,
    //    PAGE_READWRITE);

    // #3. �Ҵ� ���� �޸𸮿� myhack.dll ���("c:\\myhack.dll")�� ����.
    WriteProcessMemory(hProcess, pRemoteBuf, (LPVOID)szDllPath, dwBufSize,
        NULL);

    // #4. LoadLibraryW API �ּҸ� ���Ѵ�.
    hMod = GetModuleHandle("kernel32.dll");

    //ANSI�� �����غ�
    pThreadProc =
        (LPTHREAD_START_ROUTINE)GetProcAddress(
            GetModuleHandle("kernel32.dll"), "LoadLibraryA");

    /*�̽�������..
    pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hMod,
                                                        "LoadLibraryW");
                                                        */


                                                        // #5. notepad.exe ���μ����� �����带 ����
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
