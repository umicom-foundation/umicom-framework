/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/finding.h
 *
 * PURPOSE:
 *   Define the stable, self-contained diagnostic record produced by every
 *   CodeGuard rule.
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
#ifndef UMICOM_CODEGUARD_FINDING_H
#define UMICOM_CODEGUARD_FINDING_H
#include <stddef.h>
#include "umicom/codeguard/severity.h"
#include "umicom/codeguard/category.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CODEGUARD_RULE_ID_CAPACITY 64U
#define UMI_CODEGUARD_PATH_CAPACITY 1024U
#define UMI_CODEGUARD_MESSAGE_CAPACITY 320U
#define UMI_CODEGUARD_REMEDIATION_CAPACITY 384U
#define UMI_CODEGUARD_CWE_CAPACITY 24U
typedef struct UmiCodeGuardFinding {
    char rule_id[UMI_CODEGUARD_RULE_ID_CAPACITY];
    char cwe[UMI_CODEGUARD_CWE_CAPACITY];
    UmiCodeGuardSeverity severity;
    UmiCodeGuardCategory category;
    unsigned confidence;
    char path[UMI_CODEGUARD_PATH_CAPACITY];
    size_t line;
    size_t column;
    char message[UMI_CODEGUARD_MESSAGE_CAPACITY];
    char remediation[UMI_CODEGUARD_REMEDIATION_CAPACITY];
} UmiCodeGuardFinding;
#ifdef __cplusplus
}
#endif
#endif
