/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/rules/format.c
 *
 * PURPOSE:
 *   Provide deterministic format vulnerability patterns for CodeGuard.
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
#include "umicom/codeguard/rules/format.h"

static const UmiCodeGuardRule UMI_RULES[] = {
    {"CODEGUARD-C-FMT-001", "CWE-134", UMI_CODEGUARD_HIGH, UMI_CODEGUARD_CATEGORY_FORMAT, 88U, UMI_CODEGUARD_MATCH_CODE, "printf(input)", "Formatted output must never treat untrusted text as the format string.", "Use a constant format string such as printf(\"%s\", value) for external text."},
    {"CODEGUARD-C-FMT-002", "CWE-134", UMI_CODEGUARD_HIGH, UMI_CODEGUARD_CATEGORY_FORMAT, 88U, UMI_CODEGUARD_MATCH_CODE, "fprintf(stderr, input)", "Formatted output should use a constant format string for external data.", "Pass external strings as arguments to a fixed format string."}
};

const UmiCodeGuardRule *umi_codeguard_rules_format(size_t *out_count)
{
    if (out_count != NULL) *out_count = sizeof(UMI_RULES) / sizeof(UMI_RULES[0]);
    return UMI_RULES;
}
