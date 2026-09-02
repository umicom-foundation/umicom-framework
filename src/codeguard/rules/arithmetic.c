/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/rules/arithmetic.c
 *
 * PURPOSE:
 *   Provide deterministic arithmetic vulnerability patterns for CodeGuard.
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
#include "umicom/codeguard/rules/arithmetic.h"

static const UmiCodeGuardRule UMI_RULES[] = {
    {"CODEGUARD-C-ARITH-001", "CWE-369", UMI_CODEGUARD_LOW, UMI_CODEGUARD_CATEGORY_INTEGER, 55U, UMI_CODEGUARD_MATCH_CODE, "/ 0", "Literal division by zero detected.", "Remove the zero divisor and validate computed denominators."}
};

/*
 * Provide the codeguard rules arithmetic operation used by this module and its client
 * applications.
 */
const UmiCodeGuardRule *umi_codeguard_rules_arithmetic(size_t *out_count)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_count != NULL) *out_count = sizeof(UMI_RULES) / sizeof(UMI_RULES[0]);
    return UMI_RULES;
}
