/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/rule.h
 *
 * PURPOSE:
 *   Describe one deterministic CodeGuard rule and how it should match source
 *   text.
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
#ifndef UMICOM_CODEGUARD_RULE_H
#define UMICOM_CODEGUARD_RULE_H
#include <stddef.h>
#include "umicom/codeguard/finding.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * List the named code guard match mode values accepted by this public contract.
 */
typedef enum UmiCodeGuardMatchMode {
    UMI_CODEGUARD_MATCH_CODE = 0,
    UMI_CODEGUARD_MATCH_RAW = 1,
    UMI_CODEGUARD_MATCH_INCLUDE = 2
} UmiCodeGuardMatchMode;
/**
 * Represent the code guard rule data shared with callers of this public contract.
 */
typedef struct UmiCodeGuardRule {
    const char *rule_id;
    const char *cwe;
    UmiCodeGuardSeverity severity;
    UmiCodeGuardCategory category;
    unsigned confidence;
    UmiCodeGuardMatchMode match_mode;
    const char *needle;
    const char *message;
    const char *remediation;
} UmiCodeGuardRule;
#ifdef __cplusplus
}
#endif
#endif
