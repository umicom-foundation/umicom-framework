/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/boot_media/win32.h
 * PURPOSE: Present media inspection and transfer controls through the Windows adapter.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Native presentation boundary; the media service itself has no HWND state. */
#ifndef UMICOM_BOOT_MEDIA_WIN32_H
#define UMICOM_BOOT_MEDIA_WIN32_H
#ifdef __cplusplus
extern "C" {
#endif
int UmiBootMediaWindowsMain(void *instance,int show,int checkOnly);
#ifdef __cplusplus
}
#endif
#endif
