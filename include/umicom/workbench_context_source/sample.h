/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/sample.h
 *
 * PURPOSE:
 *   Represent one real UI/model interaction before it is normalised into the Workbench Context Event pipeline.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_SAMPLE_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_SAMPLE_H

#include "umicom/workbench_context_source/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextSourceSample {
    uint32_t structure_size;
    char sample_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char application_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char panel_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char workspace_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char secondary_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char path[UMI_WORKBENCH_CONTEXT_SOURCE_PATH_CAPACITY];
    char symbol[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    UmiWorkbenchContextSourceKind source_kind;
    UmiWorkbenchContextSourceTrigger trigger;
    UmiContextKind context_kind;
    UmiWorkbenchContextEventMetadata metadata[UMI_WORKBENCH_CONTEXT_SOURCE_MAX_METADATA];
    size_t metadata_count;
    uint32_t line;
    uint32_t column;
    uint32_t selection_length;
    int64_t signed_value;
    uint64_t unsigned_value;
    bool boolean_value;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
    uint64_t content_hash;
} UmiWorkbenchContextSourceSample;

void umi_workbench_context_source_sample_init(
    UmiWorkbenchContextSourceSample *sample,
    UmiWorkbenchContextSourceKind source_kind,
    UmiWorkbenchContextSourceTrigger trigger,
    UmiContextKind context_kind,
    const char *sample_id);
UmiStatus umi_workbench_context_source_sample_set_identity(
    UmiWorkbenchContextSourceSample *sample,
    const char *source_id,
    const char *application_id,
    const char *panel_id,
    const char *workspace_id);
UmiStatus umi_workbench_context_source_sample_set_subject(
    UmiWorkbenchContextSourceSample *sample,
    const char *subject_id,
    const char *secondary_id);
UmiStatus umi_workbench_context_source_sample_set_location(
    UmiWorkbenchContextSourceSample *sample,
    const char *path,
    const char *symbol,
    uint32_t line,
    uint32_t column,
    uint32_t selection_length);
UmiStatus umi_workbench_context_source_sample_add_metadata(
    UmiWorkbenchContextSourceSample *sample,
    const char *name,
    const char *value);
const UmiWorkbenchContextEventMetadata *
umi_workbench_context_source_sample_find_metadata(
    const UmiWorkbenchContextSourceSample *sample,
    const char *name);
UmiStatus umi_workbench_context_source_sample_validate(
    const UmiWorkbenchContextSourceSample *sample);
uint64_t umi_workbench_context_source_sample_refresh_hash(
    UmiWorkbenchContextSourceSample *sample);

#ifdef __cplusplus
}
#endif
#endif
