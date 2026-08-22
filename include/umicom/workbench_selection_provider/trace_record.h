/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/trace_record.h
 *
 * PURPOSE:
 *   Define the reusable provider trace record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_TRACE_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_TRACE_RECORD_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionProviderTraceRecord {
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
} UmiWorkbenchSelectionProviderTraceRecord;

void umi_workbench_selection_provider_trace_record_init(
    UmiWorkbenchSelectionProviderTraceRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_provider_trace_record_validate(
    const UmiWorkbenchSelectionProviderTraceRecord *record);
UmiStatus umi_workbench_selection_provider_trace_record_set_provider(
    UmiWorkbenchSelectionProviderTraceRecord *record,
    const char *provider_id);
UmiStatus umi_workbench_selection_provider_trace_record_set_source(
    UmiWorkbenchSelectionProviderTraceRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_provider_trace_record_set_subject(
    UmiWorkbenchSelectionProviderTraceRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_provider_trace_record_set_related(
    UmiWorkbenchSelectionProviderTraceRecord *record,
    const char *related_id);
UmiStatus umi_workbench_selection_provider_trace_record_set_group(
    UmiWorkbenchSelectionProviderTraceRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_provider_trace_record_set_description(
    UmiWorkbenchSelectionProviderTraceRecord *record,
    const char *description);
uint64_t umi_workbench_selection_provider_trace_record_hash(
    const UmiWorkbenchSelectionProviderTraceRecord *record);
void umi_workbench_selection_provider_trace_record_touch(
    UmiWorkbenchSelectionProviderTraceRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
