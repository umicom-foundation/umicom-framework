/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/rules/sql.c
 *
 * PURPOSE:
 *   Provide deterministic sql vulnerability patterns for CodeGuard.
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
#include "umicom/codeguard/rules/sql.h"

static const UmiCodeGuardRule UMI_RULES[] = {
    {"CODEGUARD-C-SQL-001", "CWE-89", UMI_CODEGUARD_HIGH, UMI_CODEGUARD_CATEGORY_DATABASE, 85U, UMI_CODEGUARD_MATCH_CODE, "sqlite3_exec(", "Direct SQL execution needs strict ownership and parameterisation.", "Keep SQL inside the Data Server adapter and parameterise external values."}
};

const UmiCodeGuardRule *umi_codeguard_rules_sql(size_t *out_count)
{
    if (out_count != NULL) *out_count = sizeof(UMI_RULES) / sizeof(UMI_RULES[0]);
    return UMI_RULES;
}
