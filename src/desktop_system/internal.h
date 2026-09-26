/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop_system/internal.h
 *
 * Author: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DESKTOP_SYSTEM_INTERNAL_H
#define UMICOM_DESKTOP_SYSTEM_INTERNAL_H
#include "umicom/desktop_system/monitor.h"
int UmiDesktopSystemAdd(uint64_t a, uint64_t b, uint64_t *out);
int UmiDesktopSystemMultiply(uint64_t a, uint64_t b, uint64_t *out);
void UmiDesktopSystemInitialise(UmiDesktopSystemSnapshot *snapshot);
void UmiDesktopSystemCaptureNative(const UmiDesktopSystemOptions *options,
    UmiDesktopSystemSnapshot *snapshot);
void UmiDesktopSystemReadBoot(const char *path, UmiDesktopSystemSnapshot *snapshot);
/* Only regular files (including proc pseudo-files), no symlinks or pipes. */
UmiStatus UmiDesktopSystemReadText(const char *path, char *buffer, size_t capacity,
    size_t *outLength);
/* Replace control and non-ASCII bytes in kernel-provided names. This avoids
 * invalid UTF-8 and terminal escape injection, not a change to process identity. */
void UmiDesktopSystemDisplayName(char *out, size_t capacity, const char *in, size_t length);
uint64_t UmiDesktopSystemClock(void);
#endif
