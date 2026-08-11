/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/rules/coupling_rules.c
 *
 * PURPOSE:
 *   Provide deterministic coupling rules vulnerability patterns for CodeGuard.
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
#include "umicom/codeguard/rules/coupling_rules.h"

static const UmiCodeGuardRule UMI_RULES[] = {
    {"CODEGUARD-ARCH-COUPLING-001", "", UMI_CODEGUARD_MEDIUM, UMI_CODEGUARD_CATEGORY_ARCHITECTURE, 65U, UMI_CODEGUARD_MATCH_INCLUDE, "../", "Relative parent include creates fragile source-layout coupling.", "Use a public target include path and a stable module contract instead."}
};

const UmiCodeGuardRule *umi_codeguard_rules_coupling_rules(size_t *out_count)
{
    if (out_count != NULL) *out_count = sizeof(UMI_RULES) / sizeof(UMI_RULES[0]);
    return UMI_RULES;
}
