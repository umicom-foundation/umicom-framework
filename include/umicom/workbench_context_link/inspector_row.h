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

void umi_workbench_context_link_inspector_row_init(UmiWorkbenchContextLinkInspectorRow *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_inspector_row_validate(
    const UmiWorkbenchContextLinkInspectorRow *record);
UmiStatus umi_workbench_context_link_inspector_row_copy(
    UmiWorkbenchContextLinkInspectorRow *destination,
    const UmiWorkbenchContextLinkInspectorRow *source);
uint64_t umi_workbench_context_link_inspector_row_hash(
    const UmiWorkbenchContextLinkInspectorRow *record);
UmiStatus umi_workbench_context_link_inspector_row_set_primary(
    UmiWorkbenchContextLinkInspectorRow *record,
    const char *value);
UmiStatus umi_workbench_context_link_inspector_row_set_secondary(
    UmiWorkbenchContextLinkInspectorRow *record,
    const char *value);
void umi_workbench_context_link_inspector_row_touch(
    UmiWorkbenchContextLinkInspectorRow *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
