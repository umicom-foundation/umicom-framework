/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/runtime/lifecycle_evidence.h
 *
 * PURPOSE:
 *   Record bounded application lifecycle evidence for diagnostics and acceptance without owning persistence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_RUNTIME_LIFECYCLE_EVIDENCE_H
#define UMICOM_APPLICATION_RUNTIME_LIFECYCLE_EVIDENCE_H

#include "umicom/application/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the application lifecycle event data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationLifecycleEvent {
    uint64_t sequence;
    UmiApplicationLifecyclePhase phase;
    UmiStatus status;
} UmiApplicationLifecycleEvent;

/**
 * Represent the application lifecycle evidence data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationLifecycleEvidence {
    uint32_t structure_size;
    UmiApplicationLifecycleEvent events[UMI_APPLICATION_RUNTIME_MAX_LIFECYCLE_EVENTS];
    size_t event_count;
    uint64_t next_sequence;
} UmiApplicationLifecycleEvidence;

/**
 * Initialise application lifecycle evidence from caller-provided values so later
 * operations receive a known state.
 */
void umi_application_lifecycle_evidence_init(
    UmiApplicationLifecycleEvidence *evidence);
/**
 * Provide the application lifecycle evidence record operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_lifecycle_evidence_record(
    UmiApplicationLifecycleEvidence *evidence,
    UmiApplicationLifecyclePhase phase,
    UmiStatus status);
/**
 * Provide the application lifecycle evidence last operation used by this module and its
 * client applications.
 */
const UmiApplicationLifecycleEvent *umi_application_lifecycle_evidence_last(
    const UmiApplicationLifecycleEvidence *evidence);

#ifdef __cplusplus
}
#endif

#endif
