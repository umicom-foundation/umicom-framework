/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Bootstrap executable: one call into the shared native presentation adapter. */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wchar.h>
#include "umicom/setup_centre/win32.h"
int WINAPI wWinMain(HINSTANCE instance,HINSTANCE previous,PWSTR command,int show)
{
    (void)previous;
    if(command != NULL && wcscmp(command,L"--check-ui") == 0)
        return UmiSetupWin32Check(instance);
    return UmiSetupWin32Run(instance,show);
}
