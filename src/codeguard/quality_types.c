/* Umicom Framework | CodeGuard quality primitives | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/codeguard/quality_types.h"
#include <string.h>
UmiStatus umi_codeguard_quality_copy(char *destination,size_t capacity,const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U || source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination,source,length + 1U);
    return UMI_STATUS_OK;
}
const char *umi_codeguard_evidence_kind_text(UmiCodeGuardEvidenceKind kind)
{
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
const char *umi_codeguard_quality_decision_text(UmiCodeGuardQualityDecision decision)
{
    switch (decision) {
        case UMI_CODEGUARD_QUALITY_PASS: return "pass";
        case UMI_CODEGUARD_QUALITY_WARN: return "warning";
        case UMI_CODEGUARD_QUALITY_FAIL: return "fail";
        default: return "unknown";
    }
}
