/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/rules/private_header.c
 *
 * PURPOSE:
 *   Provide deterministic private header vulnerability patterns for CodeGuard.
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
#include "umicom/codeguard/rules/private_header.h"

static const UmiCodeGuardRule UMI_RULES[] = {
    {"CODEGUARD-ARCH-PRIVATE-001", "", UMI_CODEGUARD_HIGH, UMI_CODEGUARD_CATEGORY_ARCHITECTURE, 95U, UMI_CODEGUARD_MATCH_INCLUDE, "framework/src/", "A product appears to include a Framework private implementation header.", "Products must consume only public include/umicom headers or exported SDK targets."}
};

const UmiCodeGuardRule *umi_codeguard_rules_private_header(size_t *out_count)
{
    if (out_count != NULL) *out_count = sizeof(UMI_RULES) / sizeof(UMI_RULES[0]);
    return UMI_RULES;
}
