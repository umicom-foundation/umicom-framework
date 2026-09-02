/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/rules/string.c
 *
 * PURPOSE:
 *   Provide deterministic string vulnerability patterns for CodeGuard.
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
#include "umicom/codeguard/rules/string.h"

static const UmiCodeGuardRule UMI_RULES[] = {
    {"CODEGUARD-C-STR-001", "CWE-134", UMI_CODEGUARD_HIGH, UMI_CODEGUARD_CATEGORY_STRING, 90U, UMI_CODEGUARD_MATCH_CODE, "sprintf(", "sprintf writes formatted data without a destination capacity.", "Use snprintf and check both negative returns and truncation."},
    {"CODEGUARD-C-STR-002", "CWE-170", UMI_CODEGUARD_MEDIUM, UMI_CODEGUARD_CATEGORY_STRING, 82U, UMI_CODEGUARD_MATCH_CODE, "strncpy(", "strncpy can leave the destination without a terminating NUL.", "Prefer an explicit bounded copy helper that always terminates when capacity is non-zero."}
};

/*
 * Provide the codeguard rules string operation used by this module and its client
 * applications.
 */
const UmiCodeGuardRule *umi_codeguard_rules_string(size_t *out_count)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_count != NULL) *out_count = sizeof(UMI_RULES) / sizeof(UMI_RULES[0]);
    return UMI_RULES;
}
