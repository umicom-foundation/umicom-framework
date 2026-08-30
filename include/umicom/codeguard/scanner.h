/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/scanner.h
 *
 * PURPOSE:
 *   Coordinate directory traversal, per-file security analysis, architecture
 *   checks and duplicates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */
#ifndef UMICOM_CODEGUARD_SCANNER_H
#define UMICOM_CODEGUARD_SCANNER_H
#include "umicom/base/status.h"
#include "umicom/codeguard/config.h"
#include "umicom/codeguard/result.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_codeguard_scan(const UmiCodeGuardConfig *config, UmiCodeGuardResult *result);
UmiStatus umi_codeguard_scan_file(const UmiCodeGuardConfig *config, const char *path,
                                  UmiCodeGuardResult *result);
#ifdef __cplusplus
}
#endif
#endif
