/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/permission.h
 *
 * PURPOSE:
 *   Define the reusable source permission contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_PERMISSION_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_PERMISSION_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextSourcePermission {
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
} UmiWorkbenchContextSourcePermission;

void umi_workbench_context_source_permission_init(
    UmiWorkbenchContextSourcePermission *record,
    const char *record_id);
UmiStatus umi_workbench_context_source_permission_validate(
    const UmiWorkbenchContextSourcePermission *record);
UmiStatus umi_workbench_context_source_permission_set_source(
    UmiWorkbenchContextSourcePermission *record,
    const char *source_id);
UmiStatus umi_workbench_context_source_permission_set_panel(
    UmiWorkbenchContextSourcePermission *record,
    const char *panel_id);
UmiStatus umi_workbench_context_source_permission_set_subject(
    UmiWorkbenchContextSourcePermission *record,
    const char *subject_id);
UmiStatus umi_workbench_context_source_permission_set_group(
    UmiWorkbenchContextSourcePermission *record,
    const char *group_id);
UmiStatus umi_workbench_context_source_permission_set_label(
    UmiWorkbenchContextSourcePermission *record,
    const char *label);
uint64_t umi_workbench_context_source_permission_hash(
    const UmiWorkbenchContextSourcePermission *record);
void umi_workbench_context_source_permission_touch(
    UmiWorkbenchContextSourcePermission *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
