/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/definition.h
 *
 * PURPOSE:
 *   Define one configured live interaction source, including event identity, panel binding and frequency policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_DEFINITION_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_DEFINITION_H

#include "umicom/workbench_context_source/sample.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context source definition data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextSourceDefinition {
    uint32_t structure_size;
    char source_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char application_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char panel_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char display_name[UMI_WORKBENCH_CONTEXT_SOURCE_TEXT_CAPACITY];
    char preferred_group_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    UmiWorkbenchContextSourceKind source_kind;
    UmiWorkbenchContextSourceTrigger trigger;
    UmiContextKind context_kind;
    UmiWorkbenchContextEventCoalescingMode coalescing_mode;
    uint64_t coalescing_window_ms;
    uint64_t minimum_interval_ms;
    uint64_t accepted_kinds_mask;
    bool enabled;
    uint64_t revision;
} UmiWorkbenchContextSourceDefinition;

/**
 * Initialise workbench context source definition from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_source_definition_init(
    UmiWorkbenchContextSourceDefinition *definition,
    const char *source_id);
/**
 * Provide the workbench context source definition set identity operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_source_definition_set_identity(
    UmiWorkbenchContextSourceDefinition *definition,
    const char *application_id,
    const char *panel_id,
    const char *display_name);
/**
 * Provide the workbench context source definition set group operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_source_definition_set_group(
    UmiWorkbenchContextSourceDefinition *definition,
    const char *group_id);
/**
 * Check that workbench context source definition satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_source_definition_validate(
    const UmiWorkbenchContextSourceDefinition *definition);
/**
 * Provide the workbench context source definition accepts operation used by this module
 * and its client applications.
 */
bool umi_workbench_context_source_definition_accepts(
    const UmiWorkbenchContextSourceDefinition *definition,
    const UmiWorkbenchContextSourceSample *sample);

#ifdef __cplusplus
}
#endif
#endif
