/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/rules/buffer.c
 *
 * PURPOSE:
 *   Provide deterministic buffer vulnerability patterns for CodeGuard.
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
#include "umicom/codeguard/rules/buffer.h"

static const UmiCodeGuardRule UMI_RULES[] = {
    {"CODEGUARD-C-BUF-001", "CWE-120", UMI_CODEGUARD_HIGH, UMI_CODEGUARD_CATEGORY_BUFFER, 98U, UMI_CODEGUARD_MATCH_CODE, "strcpy(", "Unbounded strcpy can overflow the destination buffer.", "Use a size-aware copy helper and verify truncation and NUL termination."},
    {"CODEGUARD-C-BUF-002", "CWE-120", UMI_CODEGUARD_HIGH, UMI_CODEGUARD_CATEGORY_BUFFER, 98U, UMI_CODEGUARD_MATCH_CODE, "strcat(", "Unbounded strcat can overflow the destination buffer.", "Track remaining capacity and use an explicit bounded append operation."},
    {"CODEGUARD-C-BUF-003", "CWE-120", UMI_CODEGUARD_CRITICAL, UMI_CODEGUARD_CATEGORY_BUFFER, 99U, UMI_CODEGUARD_MATCH_CODE, "gets(", "gets cannot limit input length and is inherently unsafe.", "Replace gets with fgets or another input routine that accepts capacity."}
};

/*
 * Provide the codeguard rules buffer operation used by this module and its client
 * applications.
 */
const UmiCodeGuardRule *umi_codeguard_rules_buffer(size_t *out_count)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_count != NULL) *out_count = sizeof(UMI_RULES) / sizeof(UMI_RULES[0]);
    return UMI_RULES;
}
