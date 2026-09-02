/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/audit.h
 *
 * PURPOSE:
 *   Retain a bounded in-memory audit trail of repository-control events.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable repository-control capability. Applications
 *   remain thin consumers and must not duplicate this policy or state model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_REPOSITORY_AUDIT_H
#define UMICOM_REPOSITORY_AUDIT_H
#include "umicom/repository/event.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the repository audit data shared with callers of this public contract.
 */
typedef struct UmiRepositoryAudit {
    UmiRepositoryControlEvent events[UMI_REPOSITORY_CONTROL_EVENT_CAPACITY];
    size_t count;
    uint64_t next_sequence;
} UmiRepositoryAudit;
/**
 * Initialise repository audit from caller-provided values so later operations receive a
 * known state.
 */
void umi_repository_audit_init(UmiRepositoryAudit *audit);
/**
 * Add repository audit only after its inputs and available capacity have been checked.
 */
UmiStatus umi_repository_audit_append(
    UmiRepositoryAudit *audit,
    UmiRepositoryControlState state,
    UmiStatus status,
    const char *path,
    const char *message);
/**
 * Provide the repository audit latest operation used by this module and its client
 * applications.
 */
const UmiRepositoryControlEvent *umi_repository_audit_latest(
    const UmiRepositoryAudit *audit);
#ifdef __cplusplus
}
#endif
#endif
