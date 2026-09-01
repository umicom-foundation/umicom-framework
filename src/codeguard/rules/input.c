/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/rules/input.c
 *
 * PURPOSE:
 *   Provide deterministic input vulnerability patterns for CodeGuard.
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
#include "umicom/codeguard/rules/input.h"

static const UmiCodeGuardRule UMI_RULES[] = {
    {"CODEGUARD-C-IN-001", "CWE-20", UMI_CODEGUARD_HIGH, UMI_CODEGUARD_CATEGORY_INPUT, 88U, UMI_CODEGUARD_MATCH_CODE, "scanf(\"%s", "Unbounded %s input can overflow the destination buffer.", "Specify a maximum width or use fgets plus validated parsing."}
};

const UmiCodeGuardRule *umi_codeguard_rules_input(size_t *out_count)
{
    if (out_count != NULL) *out_count = sizeof(UMI_RULES) / sizeof(UMI_RULES[0]);
    return UMI_RULES;
}
