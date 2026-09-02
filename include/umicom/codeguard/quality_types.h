/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/quality_types.h
 *
 * PURPOSE:
 *   Publish the public quality types contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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
/**
 * List the named code guard evidence kind values accepted by this public contract.
 */
typedef enum UmiCodeGuardEvidenceKind {
    UMI_CODEGUARD_EVIDENCE_BUILD = 1,
    UMI_CODEGUARD_EVIDENCE_TEST,
    UMI_CODEGUARD_EVIDENCE_ARCHITECTURE,
    UMI_CODEGUARD_EVIDENCE_RULE,
    UMI_CODEGUARD_EVIDENCE_ABI,
    UMI_CODEGUARD_EVIDENCE_DUPLICATION
} UmiCodeGuardEvidenceKind;
/**
 * List the named code guard evidence state values accepted by this public contract.
 */
typedef enum UmiCodeGuardEvidenceState {
    UMI_CODEGUARD_EVIDENCE_PASS = 1,
    UMI_CODEGUARD_EVIDENCE_WARNING,
    UMI_CODEGUARD_EVIDENCE_FAIL
} UmiCodeGuardEvidenceState;
/**
 * List the named code guard quality decision values accepted by this public contract.
 */
typedef enum UmiCodeGuardQualityDecision {
    UMI_CODEGUARD_QUALITY_PASS = 1,
    UMI_CODEGUARD_QUALITY_WARN,
    UMI_CODEGUARD_QUALITY_FAIL
} UmiCodeGuardQualityDecision;
/**
 * List the named code guard compatibility level values accepted by this public contract.
 */
typedef enum UmiCodeGuardCompatibilityLevel {
    UMI_CODEGUARD_COMPATIBLE = 1,
    UMI_CODEGUARD_COMPATIBILITY_WARNING,
    UMI_CODEGUARD_BREAKING
} UmiCodeGuardCompatibilityLevel;
/**
 * Copy codeguard quality into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_codeguard_quality_copy(char *destination,size_t capacity,const char *source);
/**
 * Provide the codeguard evidence kind text operation used by this module and its client
 * applications.
 */
const char *umi_codeguard_evidence_kind_text(UmiCodeGuardEvidenceKind kind);
/**
 * Provide the codeguard quality decision text operation used by this module and its client
 * applications.
 */
const char *umi_codeguard_quality_decision_text(UmiCodeGuardQualityDecision decision);
#endif
