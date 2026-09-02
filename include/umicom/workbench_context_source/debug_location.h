/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/debug_location.h
 *
 * PURPOSE:
 *   Define the reusable debugger location snapshot contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_DEBUG_LOCATION_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_DEBUG_LOCATION_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context source debug location data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextSourceDebugLocation {
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
} UmiWorkbenchContextSourceDebugLocation;

/**
 * Initialise workbench context source debug location from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_source_debug_location_init(
    UmiWorkbenchContextSourceDebugLocation *record,
    const char *record_id);
/**
 * Check that workbench context source debug location satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_source_debug_location_validate(
    const UmiWorkbenchContextSourceDebugLocation *record);
/**
 * Provide the workbench context source debug location set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_debug_location_set_source(
    UmiWorkbenchContextSourceDebugLocation *record,
    const char *source_id);
/**
 * Provide the workbench context source debug location set panel operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_debug_location_set_panel(
    UmiWorkbenchContextSourceDebugLocation *record,
    const char *panel_id);
/**
 * Provide the workbench context source debug location set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_debug_location_set_subject(
    UmiWorkbenchContextSourceDebugLocation *record,
    const char *subject_id);
/**
 * Provide the workbench context source debug location set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_debug_location_set_group(
    UmiWorkbenchContextSourceDebugLocation *record,
    const char *group_id);
/**
 * Provide the workbench context source debug location set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_debug_location_set_label(
    UmiWorkbenchContextSourceDebugLocation *record,
    const char *label);
/**
 * Provide the workbench context source debug location hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_context_source_debug_location_hash(
    const UmiWorkbenchContextSourceDebugLocation *record);
/**
 * Provide the workbench context source debug location touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_source_debug_location_touch(
    UmiWorkbenchContextSourceDebugLocation *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
