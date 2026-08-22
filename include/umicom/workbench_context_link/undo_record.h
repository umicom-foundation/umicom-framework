/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/undo_record.h
 *
 * PURPOSE:
 *   Define the reusable context-link undo record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_UNDO_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_UNDO_RECORD_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkUndoRecord {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char command_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkUndoRecord;

void umi_workbench_context_link_undo_record_init(UmiWorkbenchContextLinkUndoRecord *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_undo_record_validate(
    const UmiWorkbenchContextLinkUndoRecord *record);
UmiStatus umi_workbench_context_link_undo_record_copy(
    UmiWorkbenchContextLinkUndoRecord *destination,
    const UmiWorkbenchContextLinkUndoRecord *source);
uint64_t umi_workbench_context_link_undo_record_hash(
    const UmiWorkbenchContextLinkUndoRecord *record);
UmiStatus umi_workbench_context_link_undo_record_set_primary(
    UmiWorkbenchContextLinkUndoRecord *record,
    const char *value);
UmiStatus umi_workbench_context_link_undo_record_set_secondary(
    UmiWorkbenchContextLinkUndoRecord *record,
    const char *value);
void umi_workbench_context_link_undo_record_touch(
    UmiWorkbenchContextLinkUndoRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
