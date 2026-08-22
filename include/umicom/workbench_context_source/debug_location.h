/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/debug_location.h
 *
 * PURPOSE:
 *   Define the reusable debugger location snapshot contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_DEBUG_LOCATION_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_DEBUG_LOCATION_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextSourceDebugLocation {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char panel_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char label[UMI_WORKBENCH_CONTEXT_SOURCE_TEXT_CAPACITY];
    UmiWorkbenchContextSourceKind source_kind;
    UmiWorkbenchContextSourceTrigger trigger;
    UmiWorkbenchContextSourceState state;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchContextSourceDebugLocation;

void umi_workbench_context_source_debug_location_init(
    UmiWorkbenchContextSourceDebugLocation *record,
    const char *record_id);
UmiStatus umi_workbench_context_source_debug_location_validate(
    const UmiWorkbenchContextSourceDebugLocation *record);
UmiStatus umi_workbench_context_source_debug_location_set_source(
    UmiWorkbenchContextSourceDebugLocation *record,
    const char *source_id);
UmiStatus umi_workbench_context_source_debug_location_set_panel(
    UmiWorkbenchContextSourceDebugLocation *record,
    const char *panel_id);
UmiStatus umi_workbench_context_source_debug_location_set_subject(
    UmiWorkbenchContextSourceDebugLocation *record,
    const char *subject_id);
UmiStatus umi_workbench_context_source_debug_location_set_group(
    UmiWorkbenchContextSourceDebugLocation *record,
    const char *group_id);
UmiStatus umi_workbench_context_source_debug_location_set_label(
    UmiWorkbenchContextSourceDebugLocation *record,
    const char *label);
uint64_t umi_workbench_context_source_debug_location_hash(
    const UmiWorkbenchContextSourceDebugLocation *record);
void umi_workbench_context_source_debug_location_touch(
    UmiWorkbenchContextSourceDebugLocation *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
