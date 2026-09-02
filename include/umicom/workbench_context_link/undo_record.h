/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/undo_record.h
 *
 * PURPOSE:
 *   Define the reusable context-link undo record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_UNDO_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_UNDO_RECORD_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link undo record data shared with callers of this public
 * contract.
 */
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

/**
 * Initialise workbench context link undo record from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_undo_record_init(UmiWorkbenchContextLinkUndoRecord *record,
                                           const char *identity);
/**
 * Check that workbench context link undo record satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_link_undo_record_validate(
    const UmiWorkbenchContextLinkUndoRecord *record);
/**
 * Copy workbench context link undo record into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_workbench_context_link_undo_record_copy(
    UmiWorkbenchContextLinkUndoRecord *destination,
    const UmiWorkbenchContextLinkUndoRecord *source);
/**
 * Provide the workbench context link undo record hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_link_undo_record_hash(
    const UmiWorkbenchContextLinkUndoRecord *record);
/**
 * Provide the workbench context link undo record set primary operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_link_undo_record_set_primary(
    UmiWorkbenchContextLinkUndoRecord *record,
    const char *value);
/**
 * Provide the workbench context link undo record set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_undo_record_set_secondary(
    UmiWorkbenchContextLinkUndoRecord *record,
    const char *value);
/**
 * Provide the workbench context link undo record touch operation used by this module and
 * its client applications.
 */
void umi_workbench_context_link_undo_record_touch(
    UmiWorkbenchContextLinkUndoRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
