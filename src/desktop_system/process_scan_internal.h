/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop_system/process_scan_internal.h
 * PURPOSE: Share native process enumeration between bounded catalogues and desktop monitoring.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DESKTOP_PROCESS_SCAN_INTERNAL_H
#define UMICOM_DESKTOP_PROCESS_SCAN_INTERNAL_H
#include "umicom/desktop_system/process_catalog.h"
typedef UmiStatus (*UmiDesktopProcessVisitor)(void *context,
                                              const UmiDesktopSystemProcess *process);
/* Native loops release their enumeration handles before returning any visitor
 * or cancellation error. A visitor receives only a borrowed current row. */
UmiStatus UmiDesktopProcessScanNative(const UmiDesktopProcessCaptureOptions *options,
                                      UmiDesktopProcessVisitor visitor, void *context,
                                      UmiDesktopProcessReport *report);
UmiStatus UmiDesktopProcessReadNative(uint64_t pid, UmiDesktopSystemProcess *out);
#endif
