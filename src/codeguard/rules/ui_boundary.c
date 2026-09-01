/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/rules/ui_boundary.c
 *
 * PURPOSE:
 *   Provide deterministic ui boundary vulnerability patterns for CodeGuard.
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
#include "umicom/codeguard/rules/ui_boundary.h"

static const UmiCodeGuardRule UMI_RULES[] = {
    {"CODEGUARD-ARCH-UI-001", "", UMI_CODEGUARD_HIGH, UMI_CODEGUARD_CATEGORY_ARCHITECTURE, 95U, UMI_CODEGUARD_MATCH_INCLUDE, "gtk/gtk.h", "GTK header detected; public Framework headers must remain toolkit neutral.", "Keep GTK types and lifetime inside the GTK adapter layer."}
};

const UmiCodeGuardRule *umi_codeguard_rules_ui_boundary(size_t *out_count)
{
    if (out_count != NULL) *out_count = sizeof(UMI_RULES) / sizeof(UMI_RULES[0]);
    return UMI_RULES;
}
