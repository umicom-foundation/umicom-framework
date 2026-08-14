/*-----------------------------------------------------------------------------
 * Umicom Framework - CodeGuard architecture-quality v2 shared types
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_CODEGUARD_QUALITY_TYPES_H
#define UMICOM_CODEGUARD_QUALITY_TYPES_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#define UMI_CODEGUARD_QUALITY_ID_CAPACITY 96U
#define UMI_CODEGUARD_QUALITY_PATH_CAPACITY 512U
#define UMI_CODEGUARD_QUALITY_TEXT_CAPACITY 384U
#define UMI_CODEGUARD_QUALITY_MAX_EVIDENCE 128U
#define UMI_CODEGUARD_QUALITY_MAX_POLICIES 32U
#define UMI_CODEGUARD_QUALITY_MAX_CANDIDATES 64U
#define UMI_CODEGUARD_QUALITY_MAX_SYMBOLS 128U
#define UMI_CODEGUARD_QUALITY_MAX_CHANGES 128U
#define UMI_CODEGUARD_QUALITY_MAX_ACTIONS 64U
typedef enum UmiCodeGuardEvidenceKind {
    UMI_CODEGUARD_EVIDENCE_BUILD = 1,
    UMI_CODEGUARD_EVIDENCE_TEST,
    UMI_CODEGUARD_EVIDENCE_ARCHITECTURE,
    UMI_CODEGUARD_EVIDENCE_RULE,
    UMI_CODEGUARD_EVIDENCE_ABI,
    UMI_CODEGUARD_EVIDENCE_DUPLICATION
} UmiCodeGuardEvidenceKind;
typedef enum UmiCodeGuardEvidenceState {
    UMI_CODEGUARD_EVIDENCE_PASS = 1,
    UMI_CODEGUARD_EVIDENCE_WARNING,
    UMI_CODEGUARD_EVIDENCE_FAIL
} UmiCodeGuardEvidenceState;
typedef enum UmiCodeGuardQualityDecision {
    UMI_CODEGUARD_QUALITY_PASS = 1,
    UMI_CODEGUARD_QUALITY_WARN,
    UMI_CODEGUARD_QUALITY_FAIL
} UmiCodeGuardQualityDecision;
typedef enum UmiCodeGuardCompatibilityLevel {
    UMI_CODEGUARD_COMPATIBLE = 1,
    UMI_CODEGUARD_COMPATIBILITY_WARNING,
    UMI_CODEGUARD_BREAKING
} UmiCodeGuardCompatibilityLevel;
UmiStatus umi_codeguard_quality_copy(char *destination,size_t capacity,const char *source);
const char *umi_codeguard_evidence_kind_text(UmiCodeGuardEvidenceKind kind);
const char *umi_codeguard_quality_decision_text(UmiCodeGuardQualityDecision decision);
#endif
