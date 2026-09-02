/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/code_action_projection.h
 *
 * PURPOSE:
 *   Project ranked canonical code-action query results into a deterministic
 *   contextual workbench menu without moving provider logic into frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_CODE_ACTION_PROJECTION_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_CODE_ACTION_PROJECTION_H

#include "umicom/editor/code_action_orchestration.h"
#include "umicom/editor/intelligence_workbench/code_action_menu_group.h"
#include "umicom/editor/intelligence_workbench/projection.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_INTEL_CODE_ACTION_PROJECTION_API_VERSION 1U

/**
 * Represent the editor intel code action projection data shared with callers of this
 * public contract.
 */
typedef struct UmiEditorIntelCodeActionProjection {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorIntelCodeActionMenuGroup menu;
    UmiEditorIntelApplicability applicability[UMI_EDITOR_INTEL_MAX_ITEMS];
    char provider_ids[UMI_EDITOR_INTEL_MAX_ITEMS][UMI_EDITOR_INTEL_ID_CAPACITY];
    UmiEditorCodeActionQuerySnapshot source_snapshot;
    size_t selected_index;
    uint64_t source_revision;
    uint64_t revision;
    int has_selection;
} UmiEditorIntelCodeActionProjection;

/**
 * Initialise editor intel code action projection from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_intel_code_action_projection_init(
    UmiEditorIntelCodeActionProjection *projection);
/**
 * Provide the editor intel code action projection refresh operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_intel_code_action_projection_refresh(
    UmiEditorIntelCodeActionProjection *projection,
    UmiEditorCodeActionOrchestration *orchestration);
/**
 * Provide the editor intel code action projection select operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_intel_code_action_projection_select(
    UmiEditorIntelCodeActionProjection *projection,
    size_t index);
/**
 * Find editor intel code action projection while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiEditorIntelEntry *umi_editor_intel_code_action_projection_selected(
    const UmiEditorIntelCodeActionProjection *projection);
/**
 * Check that editor intel code action projection satisfies its contract before another
 * service relies on it.
 */
int umi_editor_intel_code_action_projection_valid(
    const UmiEditorIntelCodeActionProjection *projection);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_CODE_ACTION_PROJECTION_H */
