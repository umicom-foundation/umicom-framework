/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/refactoring_plan.h
 *
 * PURPOSE:
 *   Define provider-neutral refactoring plans backed by canonical Framework
 *   workspace edits and explicit lifecycle, risk and revision policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_REFACTORING_PLAN_H
#define UMICOM_EDITOR_REFACTORING_PLAN_H

#include "umicom/editor/workspace_edit.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_REFACTORING_PLAN_API_VERSION 1U
#define UMI_EDITOR_REFACTORING_ID_CAPACITY 128U
#define UMI_EDITOR_REFACTORING_TITLE_CAPACITY 512U
#define UMI_EDITOR_REFACTORING_URI_CAPACITY 1024U

/**
 * List the named editor refactoring kind values accepted by this public contract.
 */
typedef enum UmiEditorRefactoringKind {
    UMI_EDITOR_REFACTORING_RENAME = 1,
    UMI_EDITOR_REFACTORING_EXTRACT_FUNCTION = 2,
    UMI_EDITOR_REFACTORING_EXTRACT_VARIABLE = 3,
    UMI_EDITOR_REFACTORING_EXTRACT_CONSTANT = 4,
    UMI_EDITOR_REFACTORING_INLINE = 5,
    UMI_EDITOR_REFACTORING_MOVE = 6,
    UMI_EDITOR_REFACTORING_CHANGE_SIGNATURE = 7,
    UMI_EDITOR_REFACTORING_CONVERT = 8,
    UMI_EDITOR_REFACTORING_SOURCE_ACTION = 9,
    UMI_EDITOR_REFACTORING_CUSTOM = 10
} UmiEditorRefactoringKind;

/**
 * List the named editor refactoring risk values accepted by this public contract.
 */
typedef enum UmiEditorRefactoringRisk {
    UMI_EDITOR_REFACTORING_RISK_SAFE = 1,
    UMI_EDITOR_REFACTORING_RISK_CAUTION = 2,
    UMI_EDITOR_REFACTORING_RISK_UNSAFE = 3
} UmiEditorRefactoringRisk;

/**
 * List the named editor refactoring plan state values accepted by this public contract.
 */
typedef enum UmiEditorRefactoringPlanState {
    UMI_EDITOR_REFACTORING_PLAN_EMPTY = 1,
    UMI_EDITOR_REFACTORING_PLAN_COLLECTING = 2,
    UMI_EDITOR_REFACTORING_PLAN_READY = 3,
    UMI_EDITOR_REFACTORING_PLAN_CONFLICT = 4,
    UMI_EDITOR_REFACTORING_PLAN_APPLIED = 5,
    UMI_EDITOR_REFACTORING_PLAN_CANCELLED = 6
} UmiEditorRefactoringPlanState;

/**
 * Represent the editor refactoring descriptor data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorRefactoringDescriptor {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_EDITOR_REFACTORING_ID_CAPACITY];
    char action_id[UMI_EDITOR_REFACTORING_ID_CAPACITY];
    char provider_id[UMI_EDITOR_REFACTORING_ID_CAPACITY];
    char title[UMI_EDITOR_REFACTORING_TITLE_CAPACITY];
    char document_uri[UMI_EDITOR_REFACTORING_URI_CAPACITY];
    char symbol_id[UMI_EDITOR_REFACTORING_ID_CAPACITY];
    UmiEditorRefactoringKind kind;
    UmiEditorRefactoringRisk risk;
    uint64_t start_byte_offset;
    uint64_t end_byte_offset;
    uint64_t document_revision;
    int require_matching_revision;
    int supports_preview;
} UmiEditorRefactoringDescriptor;

/**
 * Represent the editor refactoring plan snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorRefactoringPlanSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorRefactoringDescriptor descriptor;
    UmiEditorRefactoringPlanState state;
    size_t edit_count;
    size_t document_count;
    size_t conflict_count;
    size_t required_edit_count;
    uint64_t revision;
} UmiEditorRefactoringPlanSnapshot;

/**
 * Represent the editor refactoring plan data shared with callers of this public contract.
 */
typedef struct UmiEditorRefactoringPlan UmiEditorRefactoringPlan;

/**
 * Initialise editor refactoring plan from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_refactoring_plan_create(
    UmiEditorRefactoringPlan **out_plan);
/**
 * Release or reset state held by editor refactoring plan so the same storage can be reused
 * safely.
 */
void umi_editor_refactoring_plan_destroy(UmiEditorRefactoringPlan *plan);
/**
 * Provide the editor refactoring plan begin operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_refactoring_plan_begin(
    UmiEditorRefactoringPlan *plan,
    const UmiEditorRefactoringDescriptor *descriptor);
/**
 * Provide the editor refactoring plan add edit operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_refactoring_plan_add_edit(
    UmiEditorRefactoringPlan *plan,
    const UmiEditorWorkspaceTextEdit *edit);
/**
 * Provide the editor refactoring plan finalize operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_refactoring_plan_finalize(UmiEditorRefactoringPlan *plan);
/**
 * Provide the editor refactoring plan mark applied operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_refactoring_plan_mark_applied(
    UmiEditorRefactoringPlan *plan);
/**
 * Provide the editor refactoring plan cancel operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_refactoring_plan_cancel(UmiEditorRefactoringPlan *plan);
/**
 * Copy editor refactoring plan edit into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiEditorWorkspaceEditSet *umi_editor_refactoring_plan_edit_set(
    UmiEditorRefactoringPlan *plan);
/**
 * Provide the editor refactoring plan edit set const operation used by this module and its
 * client applications.
 */
const UmiEditorWorkspaceEditSet *umi_editor_refactoring_plan_edit_set_const(
    const UmiEditorRefactoringPlan *plan);
/**
 * Provide the editor refactoring plan snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_refactoring_plan_snapshot(
    const UmiEditorRefactoringPlan *plan,
    UmiEditorRefactoringPlanSnapshot *out_snapshot);
/**
 * Provide the editor refactoring plan revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_refactoring_plan_revision(
    const UmiEditorRefactoringPlan *plan);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_REFACTORING_PLAN_H */
