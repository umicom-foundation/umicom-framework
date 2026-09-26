/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/boot_report.h
 *
 * PURPOSE:
 *   Read volatile OS boot evidence without owning privileged boot policy.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PLATFORM_BOOT_REPORT_H
#define UMICOM_PLATFORM_BOOT_REPORT_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_BOOT_REPORT_MAX_BYTES 512U
#define UMI_BOOT_REPORT_SERVICE_LIMIT 16U

typedef enum UmiBootReportMode {
    UMI_BOOT_REPORT_NORMAL = 0,
    UMI_BOOT_REPORT_REQUESTED_RECOVERY = 1
} UmiBootReportMode;
typedef enum UmiBootReportState {
    UMI_BOOT_REPORT_STARTING = 0,
    UMI_BOOT_REPORT_READY = 1,
    UMI_BOOT_REPORT_RECOVERY = 2
} UmiBootReportState;
typedef struct UmiBootReport {
    UmiBootReportMode mode;
    UmiBootReportState state;
    uint32_t planned;
    uint32_t completed;
    char reason[32];
    char sourceId[65];
} UmiBootReport;

/* Parse exactly length bytes; no I/O, allocation, execution, shell expansion or
 * retained input pointers. The result is replaced only on success. Reports are
 * observations, not authenticated identities or proof of durable health.
 * Unknown versions and fields fail closed. The schema is documented in
 * docs/platform/boot-report.md and emitted independently by minimal OS init. */
UmiStatus UmiBootReportParse(const char *text, size_t length, UmiBootReport *outReport);
const char *UmiBootReportStateText(UmiBootReportState state);
#ifdef __cplusplus
}
#endif
#endif
