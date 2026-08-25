/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/audit_event.h
 *
 * PURPOSE:
 *   Represent immutable, sequence-ordered governance events for engineering decisions and mutations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_AUDIT_EVENT_H
#define UMICOM_DEVELOPER_HELIX_AUDIT_EVENT_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiHelixAuditEvent {
    uint32_t structure_size; uint32_t api_version; char id[UMI_HELIX_ID_CAPACITY]; char subject_id[UMI_HELIX_ID_CAPACITY];
    UmiHelixState state; UmiHelixSeverity severity; uint64_t sequence; uint32_t flags; double confidence;
} UmiHelixAuditEvent;
/* Initialise a deterministic, empty audit event record. */
void umi_helix_audit_event_init(UmiHelixAuditEvent *record,const char *id);
/* Validate structural invariants before the record is consumed by another Helix component. */
UmiStatus umi_helix_audit_event_validate(const UmiHelixAuditEvent *record);
/* Return a normalised evidence score used by higher-level planning and governance. */
double umi_helix_audit_event_score(const UmiHelixAuditEvent *record);
#ifdef __cplusplus
}
#endif
#endif
