/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/boot_report_json.h
 *
 * PURPOSE:
 *   Serialize captured boot observations through an optional SDK component without executing boot policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_PLATFORM_BOOT_REPORT_JSON_H
#define UMICOM_PLATFORM_BOOT_REPORT_JSON_H
#include "umicom/platform/boot_report.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Includes the final NUL. This bound accommodates either schema below. */
#define UMI_BOOT_REPORT_JSON_CAPACITY 2048U

/* Emit schema_version=1 and scope=captured-boot-observation with one report.
 * Strings and numbers describe the validated capture, not current machine
 * health, publisher authenticity or a uniquely identified boot. No file I/O,
 * allocation, privileged operation or pointer retention occurs.
 *
 * Inputs must remain immutable and accessible throughout the call. The output
 * buffer, length and input objects must not overlap. Successful output is NUL
 * terminated; out_length excludes NUL. Failure preserves both outputs. Capacity
 * includes NUL; a too-small buffer returns CAPACITY_EXCEEDED without a prefix.
 * Link Umicom::boot_report_tools, not only Umicom::platform. */
UmiStatus UmiBootReportJson(const UmiBootReport *report,
    char *out, size_t capacity, size_t *out_length);

/* Emit scope=captured-boot-comparison, before/after reports, same_source and
 * changed_fields in mode/state/planned/completed/reason/source order. Equal
 * semantic fields yield an empty list. Changes are not graded as regressions
 * or repairs; equal source IDs do not establish the same machine or boot.
 * before and after may name the same object. Output rules match the call above. */
UmiStatus UmiBootReportComparisonJson(const UmiBootReport *before,
    const UmiBootReport *after, char *out, size_t capacity, size_t *out_length);
#ifdef __cplusplus
}
#endif
#endif
