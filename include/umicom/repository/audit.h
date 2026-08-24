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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_REPOSITORY_AUDIT_H
#define UMICOM_REPOSITORY_AUDIT_H
#include "umicom/repository/event.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRepositoryAudit {
    UmiRepositoryControlEvent events[UMI_REPOSITORY_CONTROL_EVENT_CAPACITY];
    size_t count;
    uint64_t next_sequence;
} UmiRepositoryAudit;
void umi_repository_audit_init(UmiRepositoryAudit *audit);
UmiStatus umi_repository_audit_append(
    UmiRepositoryAudit *audit,
    UmiRepositoryControlState state,
    UmiStatus status,
    const char *path,
    const char *message);
const UmiRepositoryControlEvent *umi_repository_audit_latest(
    const UmiRepositoryAudit *audit);
#ifdef __cplusplus
}
#endif
#endif
