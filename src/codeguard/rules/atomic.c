/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/rules/atomic.c
 *
 * PURPOSE:
 *   Provide deterministic atomic vulnerability patterns for CodeGuard.
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
#include "umicom/codeguard/rules/atomic.h"

static const UmiCodeGuardRule UMI_RULES[] = {
    {"CODEGUARD-C-ATOM-001", "CWE-362", UMI_CODEGUARD_LOW, UMI_CODEGUARD_CATEGORY_CONCURRENCY, 55U, UMI_CODEGUARD_MATCH_CODE, "volatile ", "volatile is not a substitute for atomic synchronisation between threads.", "Use C atomics or Framework synchronisation primitives for inter-thread state."}
};

const UmiCodeGuardRule *umi_codeguard_rules_atomic(size_t *out_count)
{
    if (out_count != NULL) *out_count = sizeof(UMI_RULES) / sizeof(UMI_RULES[0]);
    return UMI_RULES;
}
