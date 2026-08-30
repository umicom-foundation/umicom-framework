/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/rules/integer.c
 *
 * PURPOSE:
 *   Provide deterministic integer vulnerability patterns for CodeGuard.
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
#include "umicom/codeguard/rules/integer.h"

static const UmiCodeGuardRule UMI_RULES[] = {
    {"CODEGUARD-C-INT-001", "CWE-190", UMI_CODEGUARD_MEDIUM, UMI_CODEGUARD_CATEGORY_INTEGER, 68U, UMI_CODEGUARD_MATCH_CODE, "atoi(", "atoi does not report overflow or malformed input reliably.", "Use strtol/strtoul with errno, end-pointer and range validation."}
};

const UmiCodeGuardRule *umi_codeguard_rules_integer(size_t *out_count)
{
    if (out_count != NULL) *out_count = sizeof(UMI_RULES) / sizeof(UMI_RULES[0]);
    return UMI_RULES;
}
