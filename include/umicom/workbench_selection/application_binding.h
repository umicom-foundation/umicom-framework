/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/application_binding.h
 *
 * PURPOSE:
 *   Define the reusable application selection binding contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_APPLICATION_BINDING_H
#define UMICOM_WORKBENCH_SELECTION_APPLICATION_BINDING_H

#include "umicom/workbench_selection/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionApplicationBinding {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char secondary_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char label[UMI_WORKBENCH_SELECTION_TEXT_CAPACITY];
    UmiWorkbenchSelectionKind selection_kind;
    UmiWorkbenchSelectionActivation activation;
    UmiWorkbenchSelectionState state;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchSelectionApplicationBinding;

void umi_workbench_selection_application_binding_init(
    UmiWorkbenchSelectionApplicationBinding *record,
    const char *record_id);
UmiStatus umi_workbench_selection_application_binding_validate(
    const UmiWorkbenchSelectionApplicationBinding *record);
UmiStatus umi_workbench_selection_application_binding_set_source(
    UmiWorkbenchSelectionApplicationBinding *record,
    const char *source_id);
UmiStatus umi_workbench_selection_application_binding_set_subject(
    UmiWorkbenchSelectionApplicationBinding *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_application_binding_set_secondary(
    UmiWorkbenchSelectionApplicationBinding *record,
    const char *secondary_id);
UmiStatus umi_workbench_selection_application_binding_set_group(
    UmiWorkbenchSelectionApplicationBinding *record,
    const char *group_id);
UmiStatus umi_workbench_selection_application_binding_set_label(
    UmiWorkbenchSelectionApplicationBinding *record,
    const char *label);
uint64_t umi_workbench_selection_application_binding_hash(
    const UmiWorkbenchSelectionApplicationBinding *record);
void umi_workbench_selection_application_binding_touch(
    UmiWorkbenchSelectionApplicationBinding *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
