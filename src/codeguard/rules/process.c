/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/rules/process.c
 *
 * PURPOSE:
 *   Provide deterministic process vulnerability patterns for CodeGuard.
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
#include "umicom/codeguard/rules/process.h"

static const UmiCodeGuardRule UMI_RULES[] = {
    {"CODEGUARD-C-PROC-001", "CWE-78", UMI_CODEGUARD_MEDIUM, UMI_CODEGUARD_CATEGORY_COMMAND, 70U, UMI_CODEGUARD_MATCH_CODE, "execl(", "Process launch arguments must not be assembled from unchecked external strings.", "Use the Framework process service and explicit validated argv elements."}
};

const UmiCodeGuardRule *umi_codeguard_rules_process(size_t *out_count)
{
    if (out_count != NULL) *out_count = sizeof(UMI_RULES) / sizeof(UMI_RULES[0]);
    return UMI_RULES;
}
