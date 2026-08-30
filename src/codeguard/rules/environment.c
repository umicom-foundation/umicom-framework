/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/rules/environment.c
 *
 * PURPOSE:
 *   Provide deterministic environment vulnerability patterns for CodeGuard.
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
#include "umicom/codeguard/rules/environment.h"

static const UmiCodeGuardRule UMI_RULES[] = {
    {"CODEGUARD-C-ENV-001", "CWE-15", UMI_CODEGUARD_MEDIUM, UMI_CODEGUARD_CATEGORY_INPUT, 70U, UMI_CODEGUARD_MATCH_CODE, "system(getenv(", "Environment data appears to flow directly into a shell command.", "Use direct process execution and validate any environment-derived argument."}
};

const UmiCodeGuardRule *umi_codeguard_rules_environment(size_t *out_count)
{
    if (out_count != NULL) *out_count = sizeof(UMI_RULES) / sizeof(UMI_RULES[0]);
    return UMI_RULES;
}
