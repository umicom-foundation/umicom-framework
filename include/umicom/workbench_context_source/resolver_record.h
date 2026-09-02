/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/resolver_record.h
 *
 * PURPOSE:
 *   Define the reusable selection resolver record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_RESOLVER_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_RESOLVER_RECORD_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context source resolver record data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextSourceResolverRecord {
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
} UmiWorkbenchContextSourceResolverRecord;

/**
 * Initialise workbench context source resolver record from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_source_resolver_record_init(
    UmiWorkbenchContextSourceResolverRecord *record,
    const char *record_id);
/**
 * Check that workbench context source resolver record satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_context_source_resolver_record_validate(
    const UmiWorkbenchContextSourceResolverRecord *record);
/**
 * Provide the workbench context source resolver record set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_resolver_record_set_source(
    UmiWorkbenchContextSourceResolverRecord *record,
    const char *source_id);
/**
 * Provide the workbench context source resolver record set panel operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_resolver_record_set_panel(
    UmiWorkbenchContextSourceResolverRecord *record,
    const char *panel_id);
/**
 * Provide the workbench context source resolver record set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_resolver_record_set_subject(
    UmiWorkbenchContextSourceResolverRecord *record,
    const char *subject_id);
/**
 * Provide the workbench context source resolver record set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_resolver_record_set_group(
    UmiWorkbenchContextSourceResolverRecord *record,
    const char *group_id);
/**
 * Provide the workbench context source resolver record set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_resolver_record_set_label(
    UmiWorkbenchContextSourceResolverRecord *record,
    const char *label);
/**
 * Provide the workbench context source resolver record hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_context_source_resolver_record_hash(
    const UmiWorkbenchContextSourceResolverRecord *record);
/**
 * Provide the workbench context source resolver record touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_source_resolver_record_touch(
    UmiWorkbenchContextSourceResolverRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
