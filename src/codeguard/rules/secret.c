/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/rules/secret.c
 *
 * PURPOSE:
 *   Provide deterministic secret vulnerability patterns for CodeGuard.
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
#include "umicom/codeguard/rules/secret.h"

static const UmiCodeGuardRule UMI_RULES[] = {
    {"CODEGUARD-C-SEC-001", "CWE-798", UMI_CODEGUARD_CRITICAL, UMI_CODEGUARD_CATEGORY_SECRET, 92U, UMI_CODEGUARD_MATCH_RAW, "password = \"", "Possible hard-coded password literal.", "Store only a secret reference and resolve the value through the Umicom secrets service."},
    {"CODEGUARD-C-SEC-002", "CWE-798", UMI_CODEGUARD_CRITICAL, UMI_CODEGUARD_CATEGORY_SECRET, 92U, UMI_CODEGUARD_MATCH_RAW, "api_key = \"", "Possible hard-coded API key literal.", "Move API credentials outside source control and resolve them via a secret provider."}
};

const UmiCodeGuardRule *umi_codeguard_rules_secret(size_t *out_count)
{
    if (out_count != NULL) *out_count = sizeof(UMI_RULES) / sizeof(UMI_RULES[0]);
    return UMI_RULES;
}
