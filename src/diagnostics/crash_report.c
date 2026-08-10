/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/diagnostics/crash_report.c
 *
 * PURPOSE:
 *   Format and persist crash evidence using caller-selected storage locations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/diagnostics/crash_report.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>
UmiStatus umi_crash_report_init(UmiCrashReport *report, const char *application, const char *reason, const char *detail, uint64_t thread_id, uint64_t timestamp_ns)
{
    if (report == NULL || application == NULL || reason == NULL || detail == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (strlen(application) >= sizeof(report->application) || strlen(reason) >= sizeof(report->reason) || strlen(detail) >= sizeof(report->detail)) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memset(report, 0, sizeof(*report)); (void)snprintf(report->application, sizeof(report->application), "%s", application); (void)snprintf(report->reason, sizeof(report->reason), "%s", reason); (void)snprintf(report->detail, sizeof(report->detail), "%s", detail); report->thread_id = thread_id; report->timestamp_ns = timestamp_ns; return UMI_STATUS_OK;
}
UmiStatus umi_crash_report_write(const UmiCrashReport *report, const char *path)
{
    FILE *stream; int written;
    if (report == NULL || path == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    stream = fopen(path, "wb"); if (stream == NULL) return UMI_STATUS_IO_ERROR;
    written = fprintf(stream, "application=%s\nreason=%s\ndetail=%s\nthread=%llu\ntimestamp_ns=%llu\n", report->application, report->reason, report->detail, (unsigned long long)report->thread_id, (unsigned long long)report->timestamp_ns);
    if (fclose(stream) != 0 || written < 0) return UMI_STATUS_IO_ERROR;
    return UMI_STATUS_OK;
}
