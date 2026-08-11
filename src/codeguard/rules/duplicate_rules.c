/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/rules/duplicate_rules.c
 *
 * PURPOSE:
 *   Provide deterministic duplicate rules vulnerability patterns for
 *   CodeGuard.
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
#include "umicom/codeguard/rules/duplicate_rules.h"

static const UmiCodeGuardRule UMI_RULES[] = {
    {"CODEGUARD-DUP-001", "", UMI_CODEGUARD_MEDIUM, UMI_CODEGUARD_CATEGORY_DUPLICATION, 100U, UMI_CODEGUARD_MATCH_RAW, "__CODEGUARD_DUPLICATE_SENTINEL__", "Duplicate source content detected.", "Keep one authoritative implementation and make consumers depend on it."}
};

const UmiCodeGuardRule *umi_codeguard_rules_duplicate_rules(size_t *out_count)
{
    if (out_count != NULL) *out_count = sizeof(UMI_RULES) / sizeof(UMI_RULES[0]);
    return UMI_RULES;
}
