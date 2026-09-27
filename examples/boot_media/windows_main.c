/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wchar.h>
#include "umicom/boot_media/win32.h"
int WINAPI wWinMain(HINSTANCE instance,HINSTANCE previous,PWSTR command,int show)
{
    (void)previous;
    return UmiBootMediaWindowsMain(instance,show,command&&wcscmp(command,L"--check-ui")==0);
}
