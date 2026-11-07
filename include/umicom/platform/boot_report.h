/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/boot_report.h
 *
 * PURPOSE:
 *   Read and compare captured OS boot evidence without owning privileged boot policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/* Bit masks describe field changes, not severity or boot success. */
#define UMI_BOOT_REPORT_CHANGED_MODE      (UINT32_C(1) << 0)
#define UMI_BOOT_REPORT_CHANGED_STATE     (UINT32_C(1) << 1)
#define UMI_BOOT_REPORT_CHANGED_PLANNED   (UINT32_C(1) << 2)
#define UMI_BOOT_REPORT_CHANGED_COMPLETED (UINT32_C(1) << 3)
#define UMI_BOOT_REPORT_CHANGED_REASON    (UINT32_C(1) << 4)
#define UMI_BOOT_REPORT_CHANGED_SOURCE    (UINT32_C(1) << 5)

/* Validate a complete accessible report, including bounded fixed strings.
 * No I/O, allocation, ownership transfer or mutation occurs. */
UmiStatus UmiBootReportValidate(const UmiBootReport *report);

/* Validate both snapshots, then publish a change mask. Failure leaves the
 * output unchanged. Inputs may be the same object; outChanges must refer to
 * separate writable storage. Inputs must not be modified during the call.
 * Same-source reports may still come from different machines or boots. */
UmiStatus UmiBootReportCompare(const UmiBootReport *before, const UmiBootReport *after,
    uint32_t *outChanges);
const char *UmiBootReportStateText(UmiBootReportState state);
#ifdef __cplusplus
}
#endif
#endif
