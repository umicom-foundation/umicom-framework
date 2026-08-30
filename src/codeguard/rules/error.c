/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/rules/error.c
 *
 * PURPOSE:
 *   Provide deterministic error vulnerability patterns for CodeGuard.
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
#include "umicom/codeguard/rules/error.h"

static const UmiCodeGuardRule UMI_RULES[] = {
    {"CODEGUARD-C-ERR-001", "CWE-252", UMI_CODEGUARD_LOW, UMI_CODEGUARD_CATEGORY_QUALITY, 50U, UMI_CODEGUARD_MATCH_CODE, "malloc(0", "Zero-sized allocation is implementation-sensitive and often indicates a size bug.", "Validate the requested element count and byte size before allocating."}
};

const UmiCodeGuardRule *umi_codeguard_rules_error(size_t *out_count)
{
    if (out_count != NULL) *out_count = sizeof(UMI_RULES) / sizeof(UMI_RULES[0]);
    return UMI_RULES;
}
