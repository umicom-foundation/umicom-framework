/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/rules/file.c
 *
 * PURPOSE:
 *   Provide deterministic file vulnerability patterns for CodeGuard.
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
#include "umicom/codeguard/rules/file.h"

static const UmiCodeGuardRule UMI_RULES[] = {
    {"CODEGUARD-C-FILE-001", "CWE-404", UMI_CODEGUARD_LOW, UMI_CODEGUARD_CATEGORY_RESOURCE, 55U, UMI_CODEGUARD_MATCH_CODE, "fopen(\"/tmp/", "Predictable shared temporary path detected.", "Use a secure temporary-file API with exclusive creation and restrictive permissions."}
};

const UmiCodeGuardRule *umi_codeguard_rules_file(size_t *out_count)
{
    if (out_count != NULL) *out_count = sizeof(UMI_RULES) / sizeof(UMI_RULES[0]);
    return UMI_RULES;
}
