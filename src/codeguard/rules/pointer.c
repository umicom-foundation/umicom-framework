/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/rules/pointer.c
 *
 * PURPOSE:
 *   Provide deterministic pointer vulnerability patterns for CodeGuard.
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
#include "umicom/codeguard/rules/pointer.h"

static const UmiCodeGuardRule UMI_RULES[] = {
    {"CODEGUARD-C-PTR-001", "CWE-476", UMI_CODEGUARD_MEDIUM, UMI_CODEGUARD_CATEGORY_MEMORY, 60U, UMI_CODEGUARD_MATCH_CODE, "NULL->", "Literal NULL pointer dereference detected.", "Remove the dereference and prove pointer validity before access."}
};

const UmiCodeGuardRule *umi_codeguard_rules_pointer(size_t *out_count)
{
    if (out_count != NULL) *out_count = sizeof(UMI_RULES) / sizeof(UMI_RULES[0]);
    return UMI_RULES;
}
