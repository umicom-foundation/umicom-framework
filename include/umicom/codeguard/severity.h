/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/severity.h
 *
 * PURPOSE:
 *   Define CodeGuard finding severity levels and their stable text names.
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
#ifndef UMICOM_CODEGUARD_SEVERITY_H
#define UMICOM_CODEGUARD_SEVERITY_H
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiCodeGuardSeverity {
    UMI_CODEGUARD_INFO = 0,
    UMI_CODEGUARD_LOW = 1,
    UMI_CODEGUARD_MEDIUM = 2,
    UMI_CODEGUARD_HIGH = 3,
    UMI_CODEGUARD_CRITICAL = 4
} UmiCodeGuardSeverity;
const char *umi_codeguard_severity_text(UmiCodeGuardSeverity severity);
#ifdef __cplusplus
}
#endif
#endif
