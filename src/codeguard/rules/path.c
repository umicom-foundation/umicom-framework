/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/rules/path.c
 *
 * PURPOSE:
 *   Provide deterministic path vulnerability patterns for CodeGuard.
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
#include "umicom/codeguard/rules/path.h"

static const UmiCodeGuardRule UMI_RULES[] = {
    {"CODEGUARD-C-PATH-001", "CWE-22", UMI_CODEGUARD_HIGH, UMI_CODEGUARD_CATEGORY_FILESYSTEM, 88U, UMI_CODEGUARD_MATCH_CODE, "..", "Parent-directory traversal token detected in source handling.", "Canonicalise user paths, enforce an allowed root and reject escape attempts."}
};

/*
 * Provide the codeguard rules path operation used by this module and its client
 * applications.
 */
const UmiCodeGuardRule *umi_codeguard_rules_path(size_t *out_count)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_count != NULL) *out_count = sizeof(UMI_RULES) / sizeof(UMI_RULES[0]);
    return UMI_RULES;
}
