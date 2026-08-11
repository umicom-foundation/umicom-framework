/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/rules/lock.c
 *
 * PURPOSE:
 *   Provide deterministic lock vulnerability patterns for CodeGuard.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */
#include "umicom/codeguard/rules/lock.h"

static const UmiCodeGuardRule UMI_RULES[] = {
    {"CODEGUARD-C-LOCK-001", "CWE-833", UMI_CODEGUARD_LOW, UMI_CODEGUARD_CATEGORY_CONCURRENCY, 55U, UMI_CODEGUARD_MATCH_CODE, "pthread_mutex_lock(&global", "Lock acquisition requires a guaranteed unlock path.", "Use one cleanup path and keep lock scope short and ordered."}
};

const UmiCodeGuardRule *umi_codeguard_rules_lock(size_t *out_count)
{
    if (out_count != NULL) *out_count = sizeof(UMI_RULES) / sizeof(UMI_RULES[0]);
    return UMI_RULES;
}
