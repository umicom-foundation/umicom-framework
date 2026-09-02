/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/quality_types.c
 *
 * PURPOSE:
 *   Implement the quality types behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | CodeGuard quality primitives | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/codeguard/quality_types.h"
#include <string.h>
/*
 * Copy codeguard quality into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_codeguard_quality_copy(char *destination,size_t capacity,const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination,source,length + 1U);
    return UMI_STATUS_OK;
}
/*
 * Provide the codeguard evidence kind text operation used by this module and its client
 * applications.
 */
const char *umi_codeguard_evidence_kind_text(UmiCodeGuardEvidenceKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) {
        case UMI_CODEGUARD_EVIDENCE_BUILD: return "build";
        case UMI_CODEGUARD_EVIDENCE_TEST: return "test";
        case UMI_CODEGUARD_EVIDENCE_ARCHITECTURE: return "architecture";
        case UMI_CODEGUARD_EVIDENCE_RULE: return "codeguard";
        case UMI_CODEGUARD_EVIDENCE_ABI: return "abi-api";
        case UMI_CODEGUARD_EVIDENCE_DUPLICATION: return "duplication";
        default: return "unknown";
    }
}
/*
 * Provide the codeguard quality decision text operation used by this module and its client
 * applications.
 */
const char *umi_codeguard_quality_decision_text(UmiCodeGuardQualityDecision decision)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (decision) {
        case UMI_CODEGUARD_QUALITY_PASS: return "pass";
        case UMI_CODEGUARD_QUALITY_WARN: return "warning";
        case UMI_CODEGUARD_QUALITY_FAIL: return "fail";
        default: return "unknown";
    }
}
