/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/group_hint.h
 *
 * PURPOSE:
 *   Define the reusable context group routing hint contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_GROUP_HINT_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_GROUP_HINT_H
#include "umicom/workbench_context_source/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context source group hint data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextSourceGroupHint {
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
} UmiWorkbenchContextSourceGroupHint;

/**
 * Initialise workbench context source group hint from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_source_group_hint_init(
    UmiWorkbenchContextSourceGroupHint *record,
    const char *record_id);
/**
 * Check that workbench context source group hint satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_source_group_hint_validate(
    const UmiWorkbenchContextSourceGroupHint *record);
/**
 * Provide the workbench context source group hint set source operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_source_group_hint_set_source(
    UmiWorkbenchContextSourceGroupHint *record,
    const char *source_id);
/**
 * Provide the workbench context source group hint set panel operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_source_group_hint_set_panel(
    UmiWorkbenchContextSourceGroupHint *record,
    const char *panel_id);
/**
 * Provide the workbench context source group hint set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_group_hint_set_subject(
    UmiWorkbenchContextSourceGroupHint *record,
    const char *subject_id);
/**
 * Provide the workbench context source group hint set group operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_source_group_hint_set_group(
    UmiWorkbenchContextSourceGroupHint *record,
    const char *group_id);
/**
 * Provide the workbench context source group hint set label operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_source_group_hint_set_label(
    UmiWorkbenchContextSourceGroupHint *record,
    const char *label);
/**
 * Provide the workbench context source group hint hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_source_group_hint_hash(
    const UmiWorkbenchContextSourceGroupHint *record);
/**
 * Provide the workbench context source group hint touch operation used by this module and
 * its client applications.
 */
void umi_workbench_context_source_group_hint_touch(
    UmiWorkbenchContextSourceGroupHint *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
