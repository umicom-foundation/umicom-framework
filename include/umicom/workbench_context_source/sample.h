/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/sample.h
 *
 * PURPOSE:
 *   Represent one real UI/model interaction before it is normalised into the Workbench Context Event pipeline.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_SAMPLE_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_SAMPLE_H

#include "umicom/workbench_context_source/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context source sample data shared with callers of this public
 * contract.
 */
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

/**
 * Initialise workbench context source sample from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_source_sample_init(
    UmiWorkbenchContextSourceSample *sample,
    UmiWorkbenchContextSourceKind source_kind,
    UmiWorkbenchContextSourceTrigger trigger,
    UmiContextKind context_kind,
    const char *sample_id);
/**
 * Provide the workbench context source sample set identity operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_source_sample_set_identity(
    UmiWorkbenchContextSourceSample *sample,
    const char *source_id,
    const char *application_id,
    const char *panel_id,
    const char *workspace_id);
/**
 * Provide the workbench context source sample set subject operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_source_sample_set_subject(
    UmiWorkbenchContextSourceSample *sample,
    const char *subject_id,
    const char *secondary_id);
/**
 * Provide the workbench context source sample set location operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_source_sample_set_location(
    UmiWorkbenchContextSourceSample *sample,
    const char *path,
    const char *symbol,
    uint32_t line,
    uint32_t column,
    uint32_t selection_length);
/**
 * Provide the workbench context source sample add metadata operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_source_sample_add_metadata(
    UmiWorkbenchContextSourceSample *sample,
    const char *name,
    const char *value);
/**
 * Provide the workbench context source sample find metadata operation used by this module
 * and its client applications.
 */
const UmiWorkbenchContextEventMetadata *
umi_workbench_context_source_sample_find_metadata(
    const UmiWorkbenchContextSourceSample *sample,
    const char *name);
/**
 * Check that workbench context source sample satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_workbench_context_source_sample_validate(
    const UmiWorkbenchContextSourceSample *sample);
/**
 * Provide the workbench context source sample refresh hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_context_source_sample_refresh_hash(
    UmiWorkbenchContextSourceSample *sample);

#ifdef __cplusplus
}
#endif
#endif
