#define WINVER 0x0502
#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <tchar.h>
using namespace std;

#define BUFFER_SIZE 32767
#define BUFSIZE MAX_PATH

// 2 пункт
double freq = 0.0;
__int64 start = 0;

void Counter_begin()
{
    LARGE_INTEGER fr;
    if (!QueryPerformanceFrequency(&fr))
        cout << "Failed\n";

    freq = double(fr.QuadPart);
    printf("\n2.1     Frequency:     %u  Hz\n", fr);
    QueryPerformanceCounter(&fr);
    start = fr.QuadPart;
}

double Counter_after()
{
    LARGE_INTEGER fr;
    QueryPerformanceCounter(&fr);
    return double (1000000*(fr.QuadPart - start) / freq);
}

int main()
{
// 1.1 Получение версии ОС
    OSVERSIONINFO os_info = { 0 };
    os_info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&os_info);
    printf("1.1     OS version %d.%d (NT)  build %d\n", os_info.dwMajorVersion, os_info.dwMinorVersion, os_info.dwBuildNumber);

// 1.2 Получение системной директории
    TCHAR buffer[BUFFER_SIZE];
    GetSystemDirectory(buffer, BUFFER_SIZE);
    printf("1.2     System directory:     %s \n", buffer);

// 1.3 Получение названия компьютера и псевдонима текущего пользователя
    DWORD len = sizeof(buffer);
    GetComputerName(buffer,&len);
    printf("1.3     Computer name:        %s \n", buffer);
    GetUserName(buffer,&len);
    printf("        User name:            %s \n", buffer);

// 1.4 Вывод характеристик для каждого тома
    char path[MAX_PATH];
    char name[MAX_PATH];
    DWORD buf_len = MAX_PATH;
    ULARGE_INTEGER free, total;

    printf("1.4     Volume list:\n");
    HANDLE hFindVol = FindFirstVolume(name, BUFSIZE);
    do {
        printf("        ");
        printf(name);
        GetVolumePathNamesForVolumeName(name, path, buf_len, &buf_len);
        printf("\n            Path(1st): %s", path);
        if (GetDiskFreeSpaceEx(name, &free, &total, NULL)!= 0)
            {
                cout << "\n            Total size: " << total.QuadPart << " bytes" << "\n            Free space: " << free.QuadPart << " bytes\n\n";
            }
        else{
                printf("\n            No space information available.\n\n");
            }
        }
    while (FindNextVolume(hFindVol, name, BUFSIZE));
    FindVolumeClose(hFindVol);

// 1.5 Получение списка программ из реестра Windows
    TCHAR names[BUFFER_SIZE];
    TCHAR data[BUFFER_SIZE];
    DWORD size_name;
    DWORD size_data = BUFFER_SIZE;
    HKEY hkey;
    DWORD i = 0;

    printf("1.5     Startup commands:\n");
    RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_ALL_ACCESS, &hkey);
    do
    {
        size_name = sizeof(names);
        if (RegEnumValue(hkey, i, names, &size_name, NULL, REG_NONE, NULL, NULL) == ERROR_SUCCESS)
        {
            RegQueryValueEx(hkey, names, NULL, REG_NONE, (LPBYTE)data, &size_data);
            printf("            %s:  %s\n", names, data);
            i++;
        }
        else break;
    } while (1);
    RegCloseKey(hkey);

// 2 Измерение производительности ЦП
    Counter_begin();
    cout << "2.2     Measured in:   " << Counter_after()  << "  us \n";

    return 0;
}
