/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/rules/command.c
 *
 * PURPOSE:
 *   Provide deterministic command vulnerability patterns for CodeGuard.
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
#include "umicom/codeguard/rules/command.h"

static const UmiCodeGuardRule UMI_RULES[] = {
    {"CODEGUARD-C-CMD-001", "CWE-78", UMI_CODEGUARD_HIGH, UMI_CODEGUARD_CATEGORY_COMMAND, 95U, UMI_CODEGUARD_MATCH_CODE, "system(", "system executes through a command shell and can enable command injection.", "Use direct process APIs with an argv vector and validate each argument."},
    {"CODEGUARD-C-CMD-002", "CWE-78", UMI_CODEGUARD_HIGH, UMI_CODEGUARD_CATEGORY_COMMAND, 90U, UMI_CODEGUARD_MATCH_CODE, "popen(", "popen invokes a shell command and expands its attack surface.", "Prefer supervised direct process execution without a shell."}
};

const UmiCodeGuardRule *umi_codeguard_rules_command(size_t *out_count)
{
    if (out_count != NULL) *out_count = sizeof(UMI_RULES) / sizeof(UMI_RULES[0]);
    return UMI_RULES;
}
