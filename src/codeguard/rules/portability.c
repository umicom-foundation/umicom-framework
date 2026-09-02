/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/rules/portability.c
 *
 * PURPOSE:
 *   Provide deterministic portability vulnerability patterns for CodeGuard.
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
#include "umicom/codeguard/rules/portability.h"

static const UmiCodeGuardRule UMI_RULES[] = {
    {"CODEGUARD-C-PORT-001", "CWE-704", UMI_CODEGUARD_LOW, UMI_CODEGUARD_CATEGORY_PORTABILITY, 45U, UMI_CODEGUARD_MATCH_CODE, "%ld", "%ld format is ABI-width dependent when used for persisted/fixed-width values.", "Use fixed-width integer types plus the matching inttypes.h format macros."}
};

/*
 * Provide the codeguard rules portability operation used by this module and its client
 * applications.
 */
const UmiCodeGuardRule *umi_codeguard_rules_portability(size_t *out_count)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_count != NULL) *out_count = sizeof(UMI_RULES) / sizeof(UMI_RULES[0]);
    return UMI_RULES;
}
