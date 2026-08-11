/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/rules/path.c
 *
 * PURPOSE:
 *   Provide deterministic path vulnerability patterns for CodeGuard.
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
#include "umicom/codeguard/rules/path.h"

static const UmiCodeGuardRule UMI_RULES[] = {
    {"CODEGUARD-C-PATH-001", "CWE-22", UMI_CODEGUARD_HIGH, UMI_CODEGUARD_CATEGORY_FILESYSTEM, 88U, UMI_CODEGUARD_MATCH_CODE, "..", "Parent-directory traversal token detected in source handling.", "Canonicalise user paths, enforce an allowed root and reject escape attempts."}
};

const UmiCodeGuardRule *umi_codeguard_rules_path(size_t *out_count)
{
    if (out_count != NULL) *out_count = sizeof(UMI_RULES) / sizeof(UMI_RULES[0]);
    return UMI_RULES;
}
