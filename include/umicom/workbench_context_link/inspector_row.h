/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/inspector_row.h
 *
 * PURPOSE:
 *   Define the reusable context inspector row contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_INSPECTOR_ROW_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_INSPECTOR_ROW_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link inspector row data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextLinkInspectorRow {
    uint32_t structure_size;
    char row_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char label[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char value_text[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkInspectorRow;

/**
 * Initialise workbench context link inspector row from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_inspector_row_init(UmiWorkbenchContextLinkInspectorRow *record,
                                           const char *identity);
/**
 * Check that workbench context link inspector row satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_link_inspector_row_validate(
    const UmiWorkbenchContextLinkInspectorRow *record);
/**
 * Copy workbench context link inspector row into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_workbench_context_link_inspector_row_copy(
    UmiWorkbenchContextLinkInspectorRow *destination,
    const UmiWorkbenchContextLinkInspectorRow *source);
/**
 * Provide the workbench context link inspector row hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_link_inspector_row_hash(
    const UmiWorkbenchContextLinkInspectorRow *record);
/**
 * Provide the workbench context link inspector row set primary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_inspector_row_set_primary(
    UmiWorkbenchContextLinkInspectorRow *record,
    const char *value);
/**
 * Provide the workbench context link inspector row set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_inspector_row_set_secondary(
    UmiWorkbenchContextLinkInspectorRow *record,
    const char *value);
/**
 * Provide the workbench context link inspector row touch operation used by this module and
 * its client applications.
 */
void umi_workbench_context_link_inspector_row_touch(
    UmiWorkbenchContextLinkInspectorRow *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
