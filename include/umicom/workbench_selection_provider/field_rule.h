/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/field_rule.h
 *
 * PURPOSE:
 *   Define the reusable structured field rule contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_FIELD_RULE_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_FIELD_RULE_H

#include "umicom/workbench_selection_provider/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider field rule data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchSelectionProviderFieldRule {
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
} UmiWorkbenchSelectionProviderFieldRule;

/**
 * Initialise workbench selection provider field rule from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_provider_field_rule_init(
    UmiWorkbenchSelectionProviderFieldRule *record,
    const char *record_id);
/**
 * Check that workbench selection provider field rule satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_selection_provider_field_rule_validate(
    const UmiWorkbenchSelectionProviderFieldRule *record);
/**
 * Provide the workbench selection provider field rule set provider operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_field_rule_set_provider(
    UmiWorkbenchSelectionProviderFieldRule *record,
    const char *provider_id);
/**
 * Provide the workbench selection provider field rule set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_field_rule_set_source(
    UmiWorkbenchSelectionProviderFieldRule *record,
    const char *source_id);
/**
 * Provide the workbench selection provider field rule set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_field_rule_set_subject(
    UmiWorkbenchSelectionProviderFieldRule *record,
    const char *subject_id);
/**
 * Provide the workbench selection provider field rule set related operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_field_rule_set_related(
    UmiWorkbenchSelectionProviderFieldRule *record,
    const char *related_id);
/**
 * Provide the workbench selection provider field rule set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_field_rule_set_group(
    UmiWorkbenchSelectionProviderFieldRule *record,
    const char *group_id);
/**
 * Provide the workbench selection provider field rule set description operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_field_rule_set_description(
    UmiWorkbenchSelectionProviderFieldRule *record,
    const char *description);
/**
 * Provide the workbench selection provider field rule hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_selection_provider_field_rule_hash(
    const UmiWorkbenchSelectionProviderFieldRule *record);
/**
 * Provide the workbench selection provider field rule touch operation used by this module
 * and its client applications.
 */
void umi_workbench_selection_provider_field_rule_touch(
    UmiWorkbenchSelectionProviderFieldRule *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
