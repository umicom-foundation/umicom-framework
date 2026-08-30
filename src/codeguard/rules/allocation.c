/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/rules/allocation.c
 *
 * PURPOSE:
 *   Provide deterministic allocation vulnerability patterns for CodeGuard.
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
#include "umicom/codeguard/rules/allocation.h"

static const UmiCodeGuardRule UMI_RULES[] = {
    {"CODEGUARD-C-ALLOC-001", "CWE-789", UMI_CODEGUARD_MEDIUM, UMI_CODEGUARD_CATEGORY_MEMORY, 72U, UMI_CODEGUARD_MATCH_CODE, "malloc(", "Heap allocation requires overflow, failure and ownership checks.", "Validate allocation arithmetic, test for NULL and document who frees the result."},
    {"CODEGUARD-C-ALLOC-002", "CWE-401", UMI_CODEGUARD_MEDIUM, UMI_CODEGUARD_CATEGORY_MEMORY, 72U, UMI_CODEGUARD_MATCH_CODE, "calloc(", "Allocated memory must have one clear owner and cleanup path.", "Check for NULL and pair the allocation with deterministic cleanup."}
};

const UmiCodeGuardRule *umi_codeguard_rules_allocation(size_t *out_count)
{
    if (out_count != NULL) *out_count = sizeof(UMI_RULES) / sizeof(UMI_RULES[0]);
    return UMI_RULES;
}
