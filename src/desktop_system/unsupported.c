/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop_system/unsupported.c
 * PURPOSE:
 *   Expose unavailable desktop observations on unsupported host platforms.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop_system/unsupported.c
 *
 * Author: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include "process_scan_internal.h"
#include <string.h>
uint64_t UmiDesktopSystemClock(void) { return 0; }
UmiStatus UmiDesktopSystemReadText(const char *path, char *buffer, size_t capacity, size_t *length)
{ (void)path; (void)buffer; (void)capacity; (void)length; return UMI_STATUS_NOT_IMPLEMENTED; }
void UmiDesktopSystemCaptureNative(const UmiDesktopSystemOptions *options, UmiDesktopSystemSnapshot *s)
{
    (void)options; strcpy(s->source, "Unsupported host");
    s->memoryStatus = s->cpuStatus = s->processStatus = s->networkStatus = s->storageStatus = s->bootStatus = UMI_STATUS_NOT_IMPLEMENTED;
}

/* Unsupported providers return no synthetic process inventory. */
UmiStatus UmiDesktopProcessScanNative(const UmiDesktopProcessCaptureOptions *options,
    UmiDesktopProcessVisitor visitor,void *context,UmiDesktopProcessReport *report)
{
    (void)options;(void)visitor;(void)context;(void)report;return UMI_STATUS_NOT_IMPLEMENTED;
}
UmiStatus UmiDesktopProcessReadNative(uint64_t pid,UmiDesktopSystemProcess *out)
{
    (void)pid;(void)out;return UMI_STATUS_NOT_IMPLEMENTED;
}
