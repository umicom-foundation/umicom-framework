/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/rules/memory.c
 *
 * PURPOSE:
 *   Provide deterministic memory vulnerability patterns for CodeGuard.
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
#include "umicom/codeguard/rules/memory.h"

static const UmiCodeGuardRule UMI_RULES[] = {
    {"CODEGUARD-C-MEM-001", "CWE-415", UMI_CODEGUARD_HIGH, UMI_CODEGUARD_CATEGORY_MEMORY, 95U, UMI_CODEGUARD_MATCH_CODE, "free(&", "Address-of expression passed to free is almost certainly not an owned heap pointer.", "Free only the original heap allocation returned by an allocator."},
    {"CODEGUARD-C-MEM-002", "CWE-562", UMI_CODEGUARD_HIGH, UMI_CODEGUARD_CATEGORY_MEMORY, 92U, UMI_CODEGUARD_MATCH_CODE, "return &", "A function appears to return the address of local storage.", "Return owned heap/static storage or copy the value into caller-owned memory."}
};

const UmiCodeGuardRule *umi_codeguard_rules_memory(size_t *out_count)
{
    if (out_count != NULL) *out_count = sizeof(UMI_RULES) / sizeof(UMI_RULES[0]);
    return UMI_RULES;
}
