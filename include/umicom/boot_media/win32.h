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
