/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/group_picker_row.h
 *
 * PURPOSE:
 *   Define the reusable group picker row contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_GROUP_PICKER_ROW_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_GROUP_PICKER_ROW_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link group picker row data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextLinkGroupPickerRow {
    uint32_t structure_size;
    char row_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char label[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkGroupPickerRow;

/**
 * Initialise workbench context link group picker row from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_group_picker_row_init(UmiWorkbenchContextLinkGroupPickerRow *record,
                                           const char *identity);
/**
 * Check that workbench context link group picker row satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_link_group_picker_row_validate(
    const UmiWorkbenchContextLinkGroupPickerRow *record);
/**
 * Copy workbench context link group picker row into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_workbench_context_link_group_picker_row_copy(
    UmiWorkbenchContextLinkGroupPickerRow *destination,
    const UmiWorkbenchContextLinkGroupPickerRow *source);
/**
 * Provide the workbench context link group picker row hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_context_link_group_picker_row_hash(
    const UmiWorkbenchContextLinkGroupPickerRow *record);
/**
 * Provide the workbench context link group picker row set primary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_group_picker_row_set_primary(
    UmiWorkbenchContextLinkGroupPickerRow *record,
    const char *value);
/**
 * Provide the workbench context link group picker row set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_group_picker_row_set_secondary(
    UmiWorkbenchContextLinkGroupPickerRow *record,
    const char *value);
/**
 * Provide the workbench context link group picker row touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_link_group_picker_row_touch(
    UmiWorkbenchContextLinkGroupPickerRow *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
