/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/rules/global_state.c
 *
 * PURPOSE:
 *   Provide deterministic global state vulnerability patterns for CodeGuard.
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
#include "umicom/codeguard/rules/global_state.h"

static const UmiCodeGuardRule UMI_RULES[] = {
    {"CODEGUARD-ARCH-GLOBAL-001", "", UMI_CODEGUARD_MEDIUM, UMI_CODEGUARD_CATEGORY_ARCHITECTURE, 58U, UMI_CODEGUARD_MATCH_CODE, "extern int ", "Extern state can create hidden coupling across modules.", "Expose state through a narrow service/capability contract instead of shared globals."}
};

const UmiCodeGuardRule *umi_codeguard_rules_global_state(size_t *out_count)
{
    if (out_count != NULL) *out_count = sizeof(UMI_RULES) / sizeof(UMI_RULES[0]);
    return UMI_RULES;
}
