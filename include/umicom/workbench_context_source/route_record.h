/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/route_record.h
 *
 * PURPOSE:
 *   Define the reusable source routing record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_ROUTE_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_ROUTE_RECORD_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context source route record data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextSourceRouteRecord {
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
} UmiWorkbenchContextSourceRouteRecord;

/**
 * Initialise workbench context source route record from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_source_route_record_init(
    UmiWorkbenchContextSourceRouteRecord *record,
    const char *record_id);
/**
 * Check that workbench context source route record satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_source_route_record_validate(
    const UmiWorkbenchContextSourceRouteRecord *record);
/**
 * Provide the workbench context source route record set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_route_record_set_source(
    UmiWorkbenchContextSourceRouteRecord *record,
    const char *source_id);
/**
 * Provide the workbench context source route record set panel operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_route_record_set_panel(
    UmiWorkbenchContextSourceRouteRecord *record,
    const char *panel_id);
/**
 * Provide the workbench context source route record set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_route_record_set_subject(
    UmiWorkbenchContextSourceRouteRecord *record,
    const char *subject_id);
/**
 * Provide the workbench context source route record set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_route_record_set_group(
    UmiWorkbenchContextSourceRouteRecord *record,
    const char *group_id);
/**
 * Provide the workbench context source route record set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_route_record_set_label(
    UmiWorkbenchContextSourceRouteRecord *record,
    const char *label);
/**
 * Provide the workbench context source route record hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_source_route_record_hash(
    const UmiWorkbenchContextSourceRouteRecord *record);
/**
 * Provide the workbench context source route record touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_source_route_record_touch(
    UmiWorkbenchContextSourceRouteRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
