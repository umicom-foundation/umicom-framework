/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/report.h
 *
 * PURPOSE:
 *   Write CodeGuard findings as human-readable text, JSON or SARIF.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */
#ifndef UMICOM_CODEGUARD_REPORT_H
#define UMICOM_CODEGUARD_REPORT_H
#include <stdio.h>
#include "umicom/base/status.h"
#include "umicom/codeguard/result.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiCodeGuardReportFormat {
    UMI_CODEGUARD_REPORT_TEXT = 0,
    UMI_CODEGUARD_REPORT_JSON = 1,
    UMI_CODEGUARD_REPORT_SARIF = 2
} UmiCodeGuardReportFormat;
UmiStatus umi_codeguard_report_write(FILE *stream, UmiCodeGuardReportFormat format,
                                     const UmiCodeGuardResult *result);
UmiStatus umi_codeguard_report_file(const char *path, UmiCodeGuardReportFormat format,
                                    const UmiCodeGuardResult *result);
#ifdef __cplusplus
}
#endif
#endif
