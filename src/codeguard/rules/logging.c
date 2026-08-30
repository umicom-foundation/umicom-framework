/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/rules/logging.c
 *
 * PURPOSE:
 *   Provide deterministic logging vulnerability patterns for CodeGuard.
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
#include "umicom/codeguard/rules/logging.h"

static const UmiCodeGuardRule UMI_RULES[] = {
    {"CODEGUARD-C-LOG-001", "CWE-532", UMI_CODEGUARD_MEDIUM, UMI_CODEGUARD_CATEGORY_SECRET, 72U, UMI_CODEGUARD_MATCH_RAW, "printf(\"%s\", password", "A password-related identifier appears on this line; ensure it is not logged.", "Redact credentials and sensitive values before diagnostics or telemetry export."}
};

const UmiCodeGuardRule *umi_codeguard_rules_logging(size_t *out_count)
{
    if (out_count != NULL) *out_count = sizeof(UMI_RULES) / sizeof(UMI_RULES[0]);
    return UMI_RULES;
}
