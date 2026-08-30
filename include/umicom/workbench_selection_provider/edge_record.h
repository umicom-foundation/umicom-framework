/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/edge_record.h
 *
 * PURPOSE:
 *   Define the reusable structured edge record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_EDGE_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_EDGE_RECORD_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionProviderEdgeRecord {
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
} UmiWorkbenchSelectionProviderEdgeRecord;

void umi_workbench_selection_provider_edge_record_init(
    UmiWorkbenchSelectionProviderEdgeRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_provider_edge_record_validate(
    const UmiWorkbenchSelectionProviderEdgeRecord *record);
UmiStatus umi_workbench_selection_provider_edge_record_set_provider(
    UmiWorkbenchSelectionProviderEdgeRecord *record,
    const char *provider_id);
UmiStatus umi_workbench_selection_provider_edge_record_set_source(
    UmiWorkbenchSelectionProviderEdgeRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_provider_edge_record_set_subject(
    UmiWorkbenchSelectionProviderEdgeRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_provider_edge_record_set_related(
    UmiWorkbenchSelectionProviderEdgeRecord *record,
    const char *related_id);
UmiStatus umi_workbench_selection_provider_edge_record_set_group(
    UmiWorkbenchSelectionProviderEdgeRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_provider_edge_record_set_description(
    UmiWorkbenchSelectionProviderEdgeRecord *record,
    const char *description);
uint64_t umi_workbench_selection_provider_edge_record_hash(
    const UmiWorkbenchSelectionProviderEdgeRecord *record);
void umi_workbench_selection_provider_edge_record_touch(
    UmiWorkbenchSelectionProviderEdgeRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
