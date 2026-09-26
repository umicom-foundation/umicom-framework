/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop_system/unsupported.c
 *
 * Author: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <string.h>
uint64_t UmiDesktopSystemClock(void) { return 0; }
UmiStatus UmiDesktopSystemReadText(const char *path, char *buffer, size_t capacity, size_t *length)
{ (void)path; (void)buffer; (void)capacity; (void)length; return UMI_STATUS_NOT_IMPLEMENTED; }
void UmiDesktopSystemCaptureNative(const UmiDesktopSystemOptions *options, UmiDesktopSystemSnapshot *s)
{
    (void)options; strcpy(s->source, "Unsupported host");
    s->memoryStatus = s->cpuStatus = s->processStatus = s->networkStatus = s->storageStatus = s->bootStatus = UMI_STATUS_NOT_IMPLEMENTED;
}
