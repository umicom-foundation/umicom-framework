/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop_system/self_memory_report.h
 *
 * PURPOSE:
 *   Serialize bounded self-process memory observations with exact integer identities, explicit unavailable metrics and checked output.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_DESKTOP_SELF_MEMORY_REPORT_H
#define UMICOM_DESKTOP_SELF_MEMORY_REPORT_H
#include "umicom/desktop_system/self_memory.h"
#include <stddef.h>
#include <stdio.h>
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_SELF_MEMORY_REPORT_CAPACITY 768U
#define UMI_SELF_MEMORY_STAGE_MAX_BYTES 63U
/* JSON version 1 is one line, including its newline and a terminating NUL.
 * written excludes the NUL. Every uint64 counter/identity is a decimal STRING,
 * so readers using floating-point JSON numbers cannot silently round it.
 * Unknown metrics are null. Stage is 1..63 ASCII letters, digits, '.', '_' or
 * '-'; it describes a lifecycle point, never a prompt or filesystem path.
 * Formatting creates no owned heap allocation, persistent history or platform
 * query. C library internals retain their own implementation behavior.
 * Failure preserves buffer and written. The two outputs must not overlap each
 * other or sample; inputs must stay readable and stable during the call. */
UmiStatus UmiDesktopSelfMemorySampleJson(const UmiSelfMemorySample *sample,
    const char *stage,char *buffer,size_t capacity,size_t *written);
/* Capture THIS process once and write/flush one complete formatted line to a
 * caller-owned byte-oriented stream. Never closes the stream. A write failure
 * may leave a partial line; IO_ERROR is not a valid complete observation.
 * The caller serializes shared stream access. No file is opened automatically.
 * Invalid stage/stream is rejected before the native capture. */
UmiStatus UmiDesktopSelfMemoryReport(const char *stage,FILE *stream);
#ifdef __cplusplus
}
#endif
#endif
