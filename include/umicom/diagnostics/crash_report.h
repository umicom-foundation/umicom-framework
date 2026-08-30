/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/diagnostics/crash_report.h
 *
 * PURPOSE:
 *   Represent bounded crash evidence and write a portable text report without
 *   requiring a GUI or remote collector.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DIAGNOSTICS_CRASH_REPORT_H
#define UMICOM_DIAGNOSTICS_CRASH_REPORT_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/path.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CRASH_TEXT_CAPACITY 512U
typedef struct UmiCrashReport { char application[128]; char reason[UMI_CRASH_TEXT_CAPACITY]; char detail[UMI_CRASH_TEXT_CAPACITY]; uint64_t thread_id; uint64_t timestamp_ns; } UmiCrashReport;
UmiStatus umi_crash_report_init(UmiCrashReport *report,
                                const char *application,
                                const char *reason,
                                const char *detail,
                                uint64_t thread_id,
                                uint64_t timestamp_ns);
UmiStatus umi_crash_report_write(const UmiCrashReport *report, const char *path);
#ifdef __cplusplus
}
#endif

#endif
