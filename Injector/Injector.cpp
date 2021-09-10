#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
using namespace std;
HANDLE hProcess;
bool getProcess() {
    HANDLE hproc = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(entry);
    do
        if (!_stricmp(entry.szExeFile, "csgo.exe")) {
            hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, entry.th32ProcessID);
            CloseHandle(hproc);
            return 1;
        }
    while (Process32Next(hproc, &entry));
    CloseHandle(hproc);
    return 0;
}
void Restore(const char* dllname, const char* procaddy) {
    char buf[5];
    HMODULE hMod = GetModuleHandle(dllname);
    if (!hMod)
        return;
    void* addy = GetProcAddress(hMod, procaddy);
    if (!addy)
        return;
    memcpy(buf, addy, 5);
    if (!buf)
        return;
    WriteProcessMemory(hProcess, addy, &buf, 5, 0);
}
void Inject(string dll) {
    dll = dll + ".dll";
    LPVOID alloc = VirtualAllocEx(hProcess, 0, dll.length() + 1, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (alloc != NULL) {
        WriteProcessMemory(hProcess, alloc, dll.c_str(), dll.length() + 1, 0);
        CreateRemoteThread(hProcess, 0, 0, (LPTHREAD_START_ROUTINE)GetProcAddress(LoadLibrary("kernel32"), "LoadLibraryA"), alloc, 0, 0);
    }
}
int main()
{
    //FreeConsole();
    if (getProcess()) {
        string dllname;
        cout << "Enter DLL name: "; cin >> dllname;
        char* fileExt;
        char szDir[256]; //dummy buffer
        GetFullPathName(dllname.c_str(), 256, szDir, &fileExt);
        Restore("kernel32.dll", "LoadLibraryA");
        Restore("ntdll.dll", "NtOpenFile");
        Inject(szDir);
        MessageBox(NULL, "csgo.exe has been bypassed GLHF!", "DUCKBYPASS", MB_OK);
    }
}