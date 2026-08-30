/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/telemetry_record.h
 *
 * PURPOSE:
 *   Define the reusable provider telemetry record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_TELEMETRY_RECORD_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_TELEMETRY_RECORD_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionProviderTelemetryRecord {
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
} UmiWorkbenchSelectionProviderTelemetryRecord;

void umi_workbench_selection_provider_telemetry_record_init(
    UmiWorkbenchSelectionProviderTelemetryRecord *record,
    const char *record_id);
UmiStatus umi_workbench_selection_provider_telemetry_record_validate(
    const UmiWorkbenchSelectionProviderTelemetryRecord *record);
UmiStatus umi_workbench_selection_provider_telemetry_record_set_provider(
    UmiWorkbenchSelectionProviderTelemetryRecord *record,
    const char *provider_id);
UmiStatus umi_workbench_selection_provider_telemetry_record_set_source(
    UmiWorkbenchSelectionProviderTelemetryRecord *record,
    const char *source_id);
UmiStatus umi_workbench_selection_provider_telemetry_record_set_subject(
    UmiWorkbenchSelectionProviderTelemetryRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_selection_provider_telemetry_record_set_related(
    UmiWorkbenchSelectionProviderTelemetryRecord *record,
    const char *related_id);
UmiStatus umi_workbench_selection_provider_telemetry_record_set_group(
    UmiWorkbenchSelectionProviderTelemetryRecord *record,
    const char *group_id);
UmiStatus umi_workbench_selection_provider_telemetry_record_set_description(
    UmiWorkbenchSelectionProviderTelemetryRecord *record,
    const char *description);
uint64_t umi_workbench_selection_provider_telemetry_record_hash(
    const UmiWorkbenchSelectionProviderTelemetryRecord *record);
void umi_workbench_selection_provider_telemetry_record_touch(
    UmiWorkbenchSelectionProviderTelemetryRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
