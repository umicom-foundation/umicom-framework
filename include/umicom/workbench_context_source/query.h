/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/query.h
 *
 * PURPOSE:
 *   Define the reusable source query contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_QUERY_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_QUERY_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context source query data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextSourceQuery {
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
} UmiWorkbenchContextSourceQuery;

/**
 * Initialise workbench context source query from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_source_query_init(
    UmiWorkbenchContextSourceQuery *record,
    const char *record_id);
/**
 * Check that workbench context source query satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_workbench_context_source_query_validate(
    const UmiWorkbenchContextSourceQuery *record);
/**
 * Provide the workbench context source query set source operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_source_query_set_source(
    UmiWorkbenchContextSourceQuery *record,
    const char *source_id);
/**
 * Provide the workbench context source query set panel operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_source_query_set_panel(
    UmiWorkbenchContextSourceQuery *record,
    const char *panel_id);
/**
 * Provide the workbench context source query set subject operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_source_query_set_subject(
    UmiWorkbenchContextSourceQuery *record,
    const char *subject_id);
/**
 * Provide the workbench context source query set group operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_source_query_set_group(
    UmiWorkbenchContextSourceQuery *record,
    const char *group_id);
/**
 * Provide the workbench context source query set label operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_source_query_set_label(
    UmiWorkbenchContextSourceQuery *record,
    const char *label);
/**
 * Provide the workbench context source query hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_context_source_query_hash(
    const UmiWorkbenchContextSourceQuery *record);
/**
 * Provide the workbench context source query touch operation used by this module and its
 * client applications.
 */
void umi_workbench_context_source_query_touch(
    UmiWorkbenchContextSourceQuery *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
