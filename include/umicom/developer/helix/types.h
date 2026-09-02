/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/types.h
 *
 * PURPOSE:
 *   Define shared Helix constants, lifecycle states, governance decisions and deterministic utility helpers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_TYPES_H
#define UMICOM_DEVELOPER_HELIX_TYPES_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_HELIX_API_VERSION 1U
#define UMI_HELIX_ID_CAPACITY 96U
#define UMI_HELIX_TEXT_CAPACITY 256U
#define UMI_HELIX_PATH_CAPACITY 512U
#define UMI_HELIX_MAX_ITEMS 64U
#define UMI_HELIX_MAX_EDGES 128U

/**
 * List the named helix state values accepted by this public contract.
 */
typedef enum UmiHelixState {
    UMI_HELIX_IDLE=0, UMI_HELIX_PLANNED=1, UMI_HELIX_READY=2, UMI_HELIX_RUNNING=3,
    UMI_HELIX_BLOCKED=4, UMI_HELIX_SUCCEEDED=5, UMI_HELIX_FAILED=6,
    UMI_HELIX_ROLLED_BACK=7, UMI_HELIX_CANCELLED=8
} UmiHelixState;
/**
 * List the named helix severity values accepted by this public contract.
 */
typedef enum UmiHelixSeverity { UMI_HELIX_INFO=0, UMI_HELIX_WARNING=1, UMI_HELIX_ERROR=2, UMI_HELIX_CRITICAL=3 } UmiHelixSeverity;
/**
 * List the named helix decision values accepted by this public contract.
 */
typedef enum UmiHelixDecision { UMI_HELIX_DECISION_UNKNOWN=0, UMI_HELIX_DECISION_ALLOW=1, UMI_HELIX_DECISION_REVIEW=2, UMI_HELIX_DECISION_DENY=3, UMI_HELIX_DECISION_ROLLBACK=4 } UmiHelixDecision;
/**
 * List the named helix autonomy mode values accepted by this public contract.
 */
typedef enum UmiHelixAutonomyMode { UMI_HELIX_AUTONOMY_MANUAL=0, UMI_HELIX_AUTONOMY_ASSISTED=1, UMI_HELIX_AUTONOMY_SUPERVISED=2, UMI_HELIX_AUTONOMY_GUARDED=3 } UmiHelixAutonomyMode;
/**
 * List the named helix change kind values accepted by this public contract.
 */
typedef enum UmiHelixChangeKind { UMI_HELIX_CHANGE_ADD=0, UMI_HELIX_CHANGE_MODIFY=1, UMI_HELIX_CHANGE_DELETE=2, UMI_HELIX_CHANGE_RENAME=3 } UmiHelixChangeKind;
/**
 * List the named helix feedback kind values accepted by this public contract.
 */
typedef enum UmiHelixFeedbackKind { UMI_HELIX_FEEDBACK_BUILD=0, UMI_HELIX_FEEDBACK_TEST=1, UMI_HELIX_FEEDBACK_COMPILER=2, UMI_HELIX_FEEDBACK_QUALITY=3, UMI_HELIX_FEEDBACK_REPOSITORY=4, UMI_HELIX_FEEDBACK_OBSERVABILITY=5 } UmiHelixFeedbackKind;

/* Copy bounded text without truncation so engineering identifiers remain deterministic. */
UmiStatus umi_helix_copy_text(char *destination,size_t capacity,const char *source);
/* Validate a stable non-empty identifier used across Helix plans and evidence. */
bool umi_helix_valid_id(const char *id);
/* Clamp a normalised engineering score to the inclusive [0,1] interval. */
double umi_helix_clamp_score(double value);
/* Compute a guarded ratio and return zero for an empty denominator. */
double umi_helix_safe_ratio(double numerator,double denominator);
/* Produce a deterministic FNV-1a fingerprint for repository-independent text evidence. */
uint64_t umi_helix_hash_text(const char *text);
/* Classify a risk score into a reusable severity used by safety and approval gates. */
UmiHelixSeverity umi_helix_severity_from_risk(double risk);
/* Convert lifecycle state to stable diagnostic text. */
const char *umi_helix_state_text(UmiHelixState state);
#ifdef __cplusplus
}
#endif
#endif
