/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/rules/temp_file.c
 *
 * PURPOSE:
 *   Provide deterministic temp file vulnerability patterns for CodeGuard.
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
#include "umicom/codeguard/rules/temp_file.h"

static const UmiCodeGuardRule UMI_RULES[] = {
    {"CODEGUARD-C-TMP-001", "CWE-377", UMI_CODEGUARD_HIGH, UMI_CODEGUARD_CATEGORY_FILESYSTEM, 95U, UMI_CODEGUARD_MATCH_CODE, "tmpnam(", "tmpnam can create predictable temporary-file races.", "Use a secure exclusive temporary-file API with restrictive permissions."}
};

const UmiCodeGuardRule *umi_codeguard_rules_temp_file(size_t *out_count)
{
    if (out_count != NULL) *out_count = sizeof(UMI_RULES) / sizeof(UMI_RULES[0]);
    return UMI_RULES;
}
