/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/boot_media/win32.h
 *
 * PURPOSE:
 *   Present native media inspection and transfer controls without putting HWND state in the media service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BOOT_MEDIA_WIN32_H
#define UMICOM_BOOT_MEDIA_WIN32_H
#ifdef __cplusplus
extern "C" {
#endif
/* Hidden checks construct controls and synthetic observations only. The
 * existing integer entry remains compatible; zero opens the normal interface. */
typedef enum UmiBootMediaWindowsMode {
    UMI_BOOT_MEDIA_WINDOWS_RUN=0,
    UMI_BOOT_MEDIA_WINDOWS_CHECK_CONTROLS=1,
    UMI_BOOT_MEDIA_WINDOWS_CHECK_SELECTION=2,
    UMI_BOOT_MEDIA_WINDOWS_CHECK_LABELS=3,
    UMI_BOOT_MEDIA_WINDOWS_CHECK_REPORTS=4
} UmiBootMediaWindowsMode;
int UmiBootMediaWindowsMain(void *instance,int show,int checkOnly);
#ifdef __cplusplus
}
#endif
#endif
