/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/rules/lifetime_rules.c
 *
 * PURPOSE:
 *   Provide deterministic lifetime rules vulnerability patterns for CodeGuard.
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
#include "umicom/codeguard/rules/lifetime_rules.h"

static const UmiCodeGuardRule UMI_RULES[] = {
    {"CODEGUARD-C-LIFE-001", "CWE-416", UMI_CODEGUARD_HIGH, UMI_CODEGUARD_CATEGORY_MEMORY, 90U, UMI_CODEGUARD_MATCH_CODE, "realloc(", "realloc can invalidate the original allocation and requires careful assignment.", "Assign realloc to a temporary pointer, check for NULL, then update the owner."}
};

const UmiCodeGuardRule *umi_codeguard_rules_lifetime_rules(size_t *out_count)
{
    if (out_count != NULL) *out_count = sizeof(UMI_RULES) / sizeof(UMI_RULES[0]);
    return UMI_RULES;
}
