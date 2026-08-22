/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/persistence_record.h
 *
 * PURPOSE:
 *   Define the reusable context-link persistence record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_PERSISTENCE_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_PERSISTENCE_RECORD_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkPersistenceRecord {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char workspace_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkPersistenceRecord;

void umi_workbench_context_link_persistence_record_init(UmiWorkbenchContextLinkPersistenceRecord *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_persistence_record_validate(
    const UmiWorkbenchContextLinkPersistenceRecord *record);
UmiStatus umi_workbench_context_link_persistence_record_copy(
    UmiWorkbenchContextLinkPersistenceRecord *destination,
    const UmiWorkbenchContextLinkPersistenceRecord *source);
uint64_t umi_workbench_context_link_persistence_record_hash(
    const UmiWorkbenchContextLinkPersistenceRecord *record);
UmiStatus umi_workbench_context_link_persistence_record_set_primary(
    UmiWorkbenchContextLinkPersistenceRecord *record,
    const char *value);
UmiStatus umi_workbench_context_link_persistence_record_set_secondary(
    UmiWorkbenchContextLinkPersistenceRecord *record,
    const char *value);
void umi_workbench_context_link_persistence_record_touch(
    UmiWorkbenchContextLinkPersistenceRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
