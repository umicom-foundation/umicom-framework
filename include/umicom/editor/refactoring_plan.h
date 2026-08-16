/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/refactoring_plan.h
 *
 * PURPOSE:
 *   Define provider-neutral refactoring plans backed by canonical Framework
 *   workspace edits and explicit lifecycle, risk and revision policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef enum UmiEditorRefactoringRisk {
    UMI_EDITOR_REFACTORING_RISK_SAFE = 1,
    UMI_EDITOR_REFACTORING_RISK_CAUTION = 2,
    UMI_EDITOR_REFACTORING_RISK_UNSAFE = 3
} UmiEditorRefactoringRisk;

typedef enum UmiEditorRefactoringPlanState {
    UMI_EDITOR_REFACTORING_PLAN_EMPTY = 1,
    UMI_EDITOR_REFACTORING_PLAN_COLLECTING = 2,
    UMI_EDITOR_REFACTORING_PLAN_READY = 3,
    UMI_EDITOR_REFACTORING_PLAN_CONFLICT = 4,
    UMI_EDITOR_REFACTORING_PLAN_APPLIED = 5,
    UMI_EDITOR_REFACTORING_PLAN_CANCELLED = 6
} UmiEditorRefactoringPlanState;

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

typedef struct UmiEditorRefactoringPlan UmiEditorRefactoringPlan;

UmiStatus umi_editor_refactoring_plan_create(
    UmiEditorRefactoringPlan **out_plan);
void umi_editor_refactoring_plan_destroy(UmiEditorRefactoringPlan *plan);
UmiStatus umi_editor_refactoring_plan_begin(
    UmiEditorRefactoringPlan *plan,
    const UmiEditorRefactoringDescriptor *descriptor);
UmiStatus umi_editor_refactoring_plan_add_edit(
    UmiEditorRefactoringPlan *plan,
    const UmiEditorWorkspaceTextEdit *edit);
UmiStatus umi_editor_refactoring_plan_finalize(UmiEditorRefactoringPlan *plan);
UmiStatus umi_editor_refactoring_plan_mark_applied(
    UmiEditorRefactoringPlan *plan);
UmiStatus umi_editor_refactoring_plan_cancel(UmiEditorRefactoringPlan *plan);
UmiEditorWorkspaceEditSet *umi_editor_refactoring_plan_edit_set(
    UmiEditorRefactoringPlan *plan);
const UmiEditorWorkspaceEditSet *umi_editor_refactoring_plan_edit_set_const(
    const UmiEditorRefactoringPlan *plan);
UmiStatus umi_editor_refactoring_plan_snapshot(
    const UmiEditorRefactoringPlan *plan,
    UmiEditorRefactoringPlanSnapshot *out_snapshot);
uint64_t umi_editor_refactoring_plan_revision(
    const UmiEditorRefactoringPlan *plan);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_REFACTORING_PLAN_H */
