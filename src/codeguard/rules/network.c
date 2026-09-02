/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/rules/network.c
 *
 * PURPOSE:
 *   Provide deterministic network vulnerability patterns for CodeGuard.
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
#include "umicom/codeguard/rules/network.h"

static const UmiCodeGuardRule UMI_RULES[] = {
    {"CODEGUARD-C-NET-001", "CWE-319", UMI_CODEGUARD_MEDIUM, UMI_CODEGUARD_CATEGORY_NETWORK, 62U, UMI_CODEGUARD_MATCH_RAW, "http://", "Plain HTTP endpoint literal detected.", "Use TLS for remote traffic and restrict plaintext HTTP to explicit local/test profiles."}
};

/*
 * Provide the codeguard rules network operation used by this module and its client
 * applications.
 */
const UmiCodeGuardRule *umi_codeguard_rules_network(size_t *out_count)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_count != NULL) *out_count = sizeof(UMI_RULES) / sizeof(UMI_RULES[0]);
    return UMI_RULES;
}
