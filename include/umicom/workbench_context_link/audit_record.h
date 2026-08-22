/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/audit_record.h
 *
 * PURPOSE:
 *   Define the reusable context-link audit record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_AUDIT_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_AUDIT_RECORD_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkAuditRecord {
    uint32_t structure_size;
    char audit_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char actor_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char action_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    UmiContextKind context_kind;
    UmiContextChannelColour colour;
    UmiWorkbenchContextLinkMode mode;
    UmiWorkbenchContextLinkState state;
    UmiWorkbenchContextLinkOrigin origin;
    UmiWorkbenchContextLinkPriority priority;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchContextLinkAuditRecord;

void umi_workbench_context_link_audit_record_init(UmiWorkbenchContextLinkAuditRecord *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_audit_record_validate(
    const UmiWorkbenchContextLinkAuditRecord *record);
UmiStatus umi_workbench_context_link_audit_record_copy(
    UmiWorkbenchContextLinkAuditRecord *destination,
    const UmiWorkbenchContextLinkAuditRecord *source);
uint64_t umi_workbench_context_link_audit_record_hash(
    const UmiWorkbenchContextLinkAuditRecord *record);
UmiStatus umi_workbench_context_link_audit_record_set_primary(
    UmiWorkbenchContextLinkAuditRecord *record,
    const char *value);
UmiStatus umi_workbench_context_link_audit_record_set_secondary(
    UmiWorkbenchContextLinkAuditRecord *record,
    const char *value);
void umi_workbench_context_link_audit_record_touch(
    UmiWorkbenchContextLinkAuditRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
