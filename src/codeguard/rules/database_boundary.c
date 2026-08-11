/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/rules/database_boundary.c
 *
 * PURPOSE:
 *   Provide deterministic database boundary vulnerability patterns for
 *   CodeGuard.
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
#include "umicom/codeguard/rules/database_boundary.h"

static const UmiCodeGuardRule UMI_RULES[] = {
    {"CODEGUARD-ARCH-DATA-001", "", UMI_CODEGUARD_HIGH, UMI_CODEGUARD_CATEGORY_ARCHITECTURE, 95U, UMI_CODEGUARD_MATCH_INCLUDE, "sqlite3.h", "Database vendor header detected outside the Data Server boundary.", "Move database-specific code into the Framework Data Server adapter."}
};

const UmiCodeGuardRule *umi_codeguard_rules_database_boundary(size_t *out_count)
{
    if (out_count != NULL) *out_count = sizeof(UMI_RULES) / sizeof(UMI_RULES[0]);
    return UMI_RULES;
}
