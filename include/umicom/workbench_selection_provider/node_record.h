/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/node_record.h
 *
 * PURPOSE:
 *   Define the reusable structured node record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_NODE_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_NODE_RECORD_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionProviderNodeRecord {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char provider_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char related_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_SELECTION_PROVIDER_ID_CAPACITY];
    char description[UMI_WORKBENCH_SELECTION_PROVIDER_TEXT_CAPACITY];
    UmiWorkbenchSelectionProviderKind provider_kind;
    UmiWorkbenchSelectionProviderRuntimeState state;
    UmiWorkbenchSelectionKind selection_kind;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t count;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchSelectionProviderNodeRecord;

void umi_workbench_selection_provider_node_record_init(
    UmiWorkbenchSelectionProviderNodeRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_provider_node_record_validate(
    const UmiWorkbenchSelectionProviderNodeRecord *record);
UmiStatus umi_workbench_selection_provider_node_record_set_provider(
    UmiWorkbenchSelectionProviderNodeRecord *record,
    const char *provider_id);
UmiStatus umi_workbench_selection_provider_node_record_set_source(
    UmiWorkbenchSelectionProviderNodeRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_provider_node_record_set_subject(
    UmiWorkbenchSelectionProviderNodeRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_provider_node_record_set_related(
    UmiWorkbenchSelectionProviderNodeRecord *record,
    const char *related_id);
UmiStatus umi_workbench_selection_provider_node_record_set_group(
    UmiWorkbenchSelectionProviderNodeRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_provider_node_record_set_description(
    UmiWorkbenchSelectionProviderNodeRecord *record,
    const char *description);
uint64_t umi_workbench_selection_provider_node_record_hash(
    const UmiWorkbenchSelectionProviderNodeRecord *record);
void umi_workbench_selection_provider_node_record_touch(
    UmiWorkbenchSelectionProviderNodeRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
