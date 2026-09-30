/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/vm_manager/windows_main.c
 * PURPOSE:
 *   The VM product is separate from the dependency-light installer bootstrap.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * The VM product is separate from the dependency-light installer bootstrap. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wchar.h>
#include "umicom/vm_manager/win32.h"
int WINAPI wWinMain(HINSTANCE instance,HINSTANCE previous,PWSTR command,int show){
    (void)previous;
    return UmiVmWin32Run(instance,show,command&&wcscmp(command,L"--check-ui")==0);
}
