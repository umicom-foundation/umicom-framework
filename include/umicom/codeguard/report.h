/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/report.h
 *
 * PURPOSE:
 *   Write CodeGuard findings as human-readable text, JSON or SARIF.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
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
/**
 * List the named code guard report format values accepted by this public contract.
 */
typedef enum UmiCodeGuardReportFormat {
    UMI_CODEGUARD_REPORT_TEXT = 0,
    UMI_CODEGUARD_REPORT_JSON = 1,
    UMI_CODEGUARD_REPORT_SARIF = 2
} UmiCodeGuardReportFormat;
/**
 * Write codeguard report in its stable representation and report capacity or input
 * failures to the caller.
 */
UmiStatus umi_codeguard_report_write(FILE *stream, UmiCodeGuardReportFormat format,
                                     const UmiCodeGuardResult *result);
/**
 * Provide the codeguard report file operation used by this module and its client
 * applications.
 */
UmiStatus umi_codeguard_report_file(const char *path, UmiCodeGuardReportFormat format,
                                    const UmiCodeGuardResult *result);
#ifdef __cplusplus
}
#endif
#endif
