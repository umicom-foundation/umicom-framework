/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/permission.h
 *
 * PURPOSE:
 *   Define the reusable source permission contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_PERMISSION_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_PERMISSION_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context source permission data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextSourcePermission {
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
} UmiWorkbenchContextSourcePermission;

/**
 * Initialise workbench context source permission from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_source_permission_init(
    UmiWorkbenchContextSourcePermission *record,
    const char *record_id);
/**
 * Check that workbench context source permission satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_source_permission_validate(
    const UmiWorkbenchContextSourcePermission *record);
/**
 * Provide the workbench context source permission set source operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_source_permission_set_source(
    UmiWorkbenchContextSourcePermission *record,
    const char *source_id);
/**
 * Provide the workbench context source permission set panel operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_source_permission_set_panel(
    UmiWorkbenchContextSourcePermission *record,
    const char *panel_id);
/**
 * Provide the workbench context source permission set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_permission_set_subject(
    UmiWorkbenchContextSourcePermission *record,
    const char *subject_id);
/**
 * Provide the workbench context source permission set group operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_source_permission_set_group(
    UmiWorkbenchContextSourcePermission *record,
    const char *group_id);
/**
 * Provide the workbench context source permission set label operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_source_permission_set_label(
    UmiWorkbenchContextSourcePermission *record,
    const char *label);
/**
 * Provide the workbench context source permission hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_source_permission_hash(
    const UmiWorkbenchContextSourcePermission *record);
/**
 * Provide the workbench context source permission touch operation used by this module and
 * its client applications.
 */
void umi_workbench_context_source_permission_touch(
    UmiWorkbenchContextSourcePermission *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
