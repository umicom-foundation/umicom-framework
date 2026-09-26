/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop_system/linux_parse.h
 *
 * Author: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DESKTOP_SYSTEM_LINUX_PARSE_H
#define UMICOM_DESKTOP_SYSTEM_LINUX_PARSE_H
#include "umicom/desktop_system/monitor.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Toolkit-neutral parsers: bounded bytes, no I/O, no retained input pointers.
 * Every output is replaced only on success. NUL inside input is rejected.
 * Required duplicate fields and arithmetic overflow fail rather than clamp. */
UmiStatus UmiDesktopSystemParseMemory(const char *text, size_t length,
    UmiDesktopSystemMemory *outMemory);
UmiStatus UmiDesktopSystemParseCpu(const char *text, size_t length,
    UmiDesktopSystemCpu *outCpu);
UmiStatus UmiDesktopSystemParseProcess(const char *text, size_t length,
    uint64_t pageSize, UmiDesktopSystemProcess *outProcess);
UmiStatus UmiDesktopSystemParseInterface(const char *text, size_t length,
    UmiDesktopSystemInterface *outInterface);
#ifdef __cplusplus
}
#endif
#endif
