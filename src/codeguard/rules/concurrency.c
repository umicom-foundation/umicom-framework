/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/rules/concurrency.c
 *
 * PURPOSE:
 *   Provide deterministic concurrency vulnerability patterns for CodeGuard.
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
#include "umicom/codeguard/rules/concurrency.h"

static const UmiCodeGuardRule UMI_RULES[] = {
    {"CODEGUARD-C-THR-001", "CWE-362", UMI_CODEGUARD_MEDIUM, UMI_CODEGUARD_CATEGORY_CONCURRENCY, 60U, UMI_CODEGUARD_MATCH_CODE, "static volatile", "Static volatile state is not sufficient synchronisation for concurrent mutation.", "Prefer instance state; otherwise document synchronisation and use atomic/locking primitives."}
};

/*
 * Provide the codeguard rules concurrency operation used by this module and its client
 * applications.
 */
const UmiCodeGuardRule *umi_codeguard_rules_concurrency(size_t *out_count)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_count != NULL) *out_count = sizeof(UMI_RULES) / sizeof(UMI_RULES[0]);
    return UMI_RULES;
}
