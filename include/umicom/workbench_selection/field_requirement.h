/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection/field_requirement.h
 *
 * PURPOSE:
 *   Define the reusable selection field requirement contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_FIELD_REQUIREMENT_H
#define UMICOM_WORKBENCH_SELECTION_FIELD_REQUIREMENT_H
#include "umicom/workbench_selection/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection field requirement data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchSelectionFieldRequirement {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char related_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    char description[UMI_WORKBENCH_SELECTION_TEXT_CAPACITY];
    UmiWorkbenchSelectionKind selection_kind;
    UmiWorkbenchSelectionActivation activation;
    UmiWorkbenchSelectionState state;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t count;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchSelectionFieldRequirement;

/**
 * Initialise workbench selection field requirement from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_field_requirement_init(
    UmiWorkbenchSelectionFieldRequirement *record,
    const char *record_id);
/**
 * Check that workbench selection field requirement satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_selection_field_requirement_validate(
    const UmiWorkbenchSelectionFieldRequirement *record);
/**
 * Provide the workbench selection field requirement set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_field_requirement_set_source(
    UmiWorkbenchSelectionFieldRequirement *record,
    const char *source_id);
/**
 * Provide the workbench selection field requirement set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_field_requirement_set_subject(
    UmiWorkbenchSelectionFieldRequirement *record,
    const char *subject_id);
/**
 * Provide the workbench selection field requirement set related operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_field_requirement_set_related(
    UmiWorkbenchSelectionFieldRequirement *record,
    const char *related_id);
/**
 * Provide the workbench selection field requirement set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_field_requirement_set_group(
    UmiWorkbenchSelectionFieldRequirement *record,
    const char *group_id);
/**
 * Provide the workbench selection field requirement set description operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_field_requirement_set_description(
    UmiWorkbenchSelectionFieldRequirement *record,
    const char *description);
/**
 * Provide the workbench selection field requirement hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_selection_field_requirement_hash(
    const UmiWorkbenchSelectionFieldRequirement *record);
/**
 * Provide the workbench selection field requirement touch operation used by this module
 * and its client applications.
 */
void umi_workbench_selection_field_requirement_touch(
    UmiWorkbenchSelectionFieldRequirement *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
