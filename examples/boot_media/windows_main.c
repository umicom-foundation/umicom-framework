/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/boot_media/windows_main.c
 *
 * PURPOSE:
 *   Start the Windows boot-media review interface or an explicit hidden-control check.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wchar.h>
#include "umicom/boot_media/win32.h"
int WINAPI wWinMain(HINSTANCE instance,HINSTANCE previous,PWSTR command,int show)
{
    (void)previous;
    /* Retain the earlier single-check entry for review. Named modes now cover
     * selection identity and complete Unicode labels through the same adapter. */
#if 0
    return UmiBootMediaWindowsMain(instance,show,command&&wcscmp(command,L"--check-ui")==0);
#endif
    int mode=UMI_BOOT_MEDIA_WINDOWS_RUN;
    if(command&&wcscmp(command,L"--check-ui")==0)mode=UMI_BOOT_MEDIA_WINDOWS_CHECK_CONTROLS;
    else if(command&&wcscmp(command,L"--check-selection")==0)mode=UMI_BOOT_MEDIA_WINDOWS_CHECK_SELECTION;
    else if(command&&wcscmp(command,L"--check-labels")==0)mode=UMI_BOOT_MEDIA_WINDOWS_CHECK_LABELS;
    else if(command&&wcscmp(command,L"--check-reports")==0)mode=UMI_BOOT_MEDIA_WINDOWS_CHECK_REPORTS;
    return UmiBootMediaWindowsMain(instance,show,mode);
}
