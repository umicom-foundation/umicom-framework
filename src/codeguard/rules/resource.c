/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/rules/resource.c
 *
 * PURPOSE:
 *   Provide deterministic resource vulnerability patterns for CodeGuard.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */
#include "umicom/codeguard/rules/resource.h"

static const UmiCodeGuardRule UMI_RULES[] = {
    {"CODEGUARD-C-RES-001", "CWE-772", UMI_CODEGUARD_LOW, UMI_CODEGUARD_CATEGORY_RESOURCE, 55U, UMI_CODEGUARD_MATCH_CODE, "CreateFileA(NULL", "Windows HANDLE creation requires CloseHandle on every terminal path.", "Wrap HANDLE ownership or centralise cleanup so every successful create is closed."}
};

const UmiCodeGuardRule *umi_codeguard_rules_resource(size_t *out_count)
{
    if (out_count != NULL) *out_count = sizeof(UMI_RULES) / sizeof(UMI_RULES[0]);
    return UMI_RULES;
}
