/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/group_picker_row.h
 *
 * PURPOSE:
 *   Define the reusable group picker row contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_GROUP_PICKER_ROW_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_GROUP_PICKER_ROW_H

#include "umicom/workbench_context_link/types.h"

#ifdef __cplusplus
extern "C" {
#endif

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

void umi_workbench_context_link_group_picker_row_init(UmiWorkbenchContextLinkGroupPickerRow *record,
                                           const char *identity);
UmiStatus umi_workbench_context_link_group_picker_row_validate(
    const UmiWorkbenchContextLinkGroupPickerRow *record);
UmiStatus umi_workbench_context_link_group_picker_row_copy(
    UmiWorkbenchContextLinkGroupPickerRow *destination,
    const UmiWorkbenchContextLinkGroupPickerRow *source);
uint64_t umi_workbench_context_link_group_picker_row_hash(
    const UmiWorkbenchContextLinkGroupPickerRow *record);
UmiStatus umi_workbench_context_link_group_picker_row_set_primary(
    UmiWorkbenchContextLinkGroupPickerRow *record,
    const char *value);
UmiStatus umi_workbench_context_link_group_picker_row_set_secondary(
    UmiWorkbenchContextLinkGroupPickerRow *record,
    const char *value);
void umi_workbench_context_link_group_picker_row_touch(
    UmiWorkbenchContextLinkGroupPickerRow *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif
