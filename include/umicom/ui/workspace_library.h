/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workspace_library.h
 * PURPOSE: Copied named-layout views and atomic library actions over the
 * existing workspace customisation owner. No second registry or persistence.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_WORKSPACE_LIBRARY_H
#define UMICOM_UI_WORKSPACE_LIBRARY_H
#include "umicom/ui/workspace_customisation.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiUiWorkspaceLibraryAction {
    UMI_UI_WORKSPACE_LIBRARY_DUPLICATE = 1,
    UMI_UI_WORKSPACE_LIBRARY_RENAME = 2,
    UMI_UI_WORKSPACE_LIBRARY_REMOVE = 3,
    /* Explicit values preserve the original action ABI. Moves exchange adjacent
     * stored rows without selecting a different layout or editing its contents. */
    UMI_UI_WORKSPACE_LIBRARY_MOVE_EARLIER = 5,
    UMI_UI_WORKSPACE_LIBRARY_MOVE_LATER = 6,
    UMI_UI_WORKSPACE_LIBRARY_ACTIVATE = 4
} UmiUiWorkspaceLibraryAction;

/* Optional owning namespace. NULL/empty is unrestricted. A supplied prefix
 * ends in '.' and must match every stored layout ID, not just the new ID.
 * Native products supply their existing canonical owning prefix. */
typedef struct UmiUiWorkspaceLibraryPolicy {
    const char *layout_prefix;
} UmiUiWorkspaceLibraryPolicy;

/* Strings are borrowed only during apply and may reference the live model.
 * target_layout_id identifies the source for Duplicate and target otherwise.
 * new_layout_id is required only for Duplicate; name for Duplicate/Rename.
 * IDs are bounded ASCII alphanumeric/._-; names are valid UTF-8 with no ASCII
 * control characters and cannot consist only of ASCII spaces.
 * Removal requires confirmed=true; there is deliberately no default exception
 * list or independent catalogue of protected layout names. */
typedef struct UmiUiWorkspaceLibraryRequest {
    UmiUiWorkspaceLibraryAction action;
    const char *target_layout_id;
    const char *new_layout_id;
    const char *name;
    uint64_t expected_customisation_revision;
    bool confirmed;
} UmiUiWorkspaceLibraryRequest;

/* Each row owns its text. window_count includes hidden stored instances. */
typedef struct UmiUiWorkspaceLibraryRow {
    char layout_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    char name[UMI_UI_WORKSPACE_LAYOUT_NAME_CAPACITY];
    size_t window_count;
    bool active;
    bool locked;
} UmiUiWorkspaceLibraryRow;

typedef struct UmiUiWorkspaceLibrarySnapshot {
    UmiUiWorkspaceLibraryRow rows[UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS];
    size_t layout_count;
    uint64_t customisation_revision;
    bool editing;
} UmiUiWorkspaceLibrarySnapshot;

/* A suggestion owns both identities and the revision the user observed.
 * It reserves nothing: submit these values to the existing DUPLICATE action,
 * together with a chosen display name, on the workspace owner's thread. */
typedef struct UmiUiWorkspaceLibraryCopySuggestion {
    char target_layout_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    char new_layout_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    uint64_t expected_customisation_revision;
} UmiUiWorkspaceLibraryCopySuggestion;

/* Suggest the first unused <source>.copy.N identity, starting at N=1, from a
 * fully validated copied library. The whole source ID is retained, preserving
 * its application namespace. Names and model state are never changed.
 * Malformed snapshots return INVALID_STATE, editing returns BUSY, a missing
 * source returns NOT_FOUND, and full libraries, exhausted revisions or IDs
 * without room for a suffix return CAPACITY_EXCEEDED. IDs are never truncated.
 * All failures leave output unchanged. Output must not overlap the snapshot;
 * target_layout_id may point into either input or output. No heap allocation,
 * storage, clock, random source or global counter is used. This is a proposal,
 * not authority: apply still checks current revision, ownership and collision. */
UmiStatus umi_ui_workspace_library_suggest_copy(
    const UmiUiWorkspaceLibrarySnapshot *snapshot,
    const char *target_layout_id,
    UmiUiWorkspaceLibraryCopySuggestion *out_suggestion);

/* Read copied rows in stable model order, including during editing. Output is
 * unchanged on error and must not overlap the model. No pointers are retained. */
UmiStatus umi_ui_workspace_library_snapshot(
    const UmiUiWorkspaceCustomisation *customisation,
    const UmiUiWorkspaceLibraryPolicy *policy,
    UmiUiWorkspaceLibrarySnapshot *out_snapshot);

/* MOVE_EARLIER/MOVE_LATER reorder the complete stored list by one position.
 * At the first/last boundary the operation succeeds without changing revisions.
 * A real move advances only the customisation revision, preserving all layout
 * revisions, active identity, geometry, locking and catalogues. Whole-library
 * Save preserves order; active-layout Save alone does not store the list. */

/* Apply one action through a heap candidate. All errors preserve the live
 * model and optional output. Active editing returns BUSY; a stale expected
 * revision returns INVALID_STATE. Wrong ownership returns PERMISSION_DENIED.
 * Duplicate never overwrites and activates the new copy. Rename preserves
 * active selection and locking. Remove never removes the last layout, and
 * active removal selects the first remaining model row. Templates/defaults
 * are user-owned copies here and may be renamed/removed under those rules.
 * No-op Rename/Activate succeeds without advancing any revision. A real
 * action publishes one customisation revision; layout revisions change only
 * for renamed/new layouts. Windows, context and template catalogues remain
 * unchanged. The optional output must not overlap the model.
 * These owner-thread operations do not save the library to disk: existing
 * active-checkpoint Save/Restore retains its established scope. */
UmiStatus umi_ui_workspace_library_apply(
    UmiUiWorkspaceCustomisation *customisation,
    const UmiUiWorkspaceLibraryPolicy *policy,
    const UmiUiWorkspaceLibraryRequest *request,
    UmiUiWorkspaceLibrarySnapshot *out_snapshot);
/* Compare copied list summaries without modifying either owner. Rows follow
 * the proposed order, then removed rows in current order. An absent side has
 * index SIZE_MAX and a zero row. MOVED means its absolute list index changes.
 * This describes names, IDs, counts, locking and active selection only; equal
 * summaries do not prove equal panel geometry, context routing or documents. */
#define UMI_UI_WORKSPACE_LIBRARY_MAX_COMPARISON_ROWS (2U * UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS)
typedef enum UmiUiWorkspaceLibraryChange {
    UMI_UI_WORKSPACE_LIBRARY_CHANGE_ADDED = 1U,
    UMI_UI_WORKSPACE_LIBRARY_CHANGE_REMOVED = 2U,
    UMI_UI_WORKSPACE_LIBRARY_CHANGE_NAME = 4U,
    UMI_UI_WORKSPACE_LIBRARY_CHANGE_POSITION = 8U,
    UMI_UI_WORKSPACE_LIBRARY_CHANGE_WINDOWS = 16U,
    UMI_UI_WORKSPACE_LIBRARY_CHANGE_LOCKED = 32U,
    UMI_UI_WORKSPACE_LIBRARY_CHANGE_ACTIVE = 64U
} UmiUiWorkspaceLibraryChange;
typedef struct UmiUiWorkspaceLibraryComparisonRow {
    UmiUiWorkspaceLibraryRow before;
    UmiUiWorkspaceLibraryRow after;
    size_t before_index;
    size_t after_index;
    uint32_t changes;
} UmiUiWorkspaceLibraryComparisonRow;
typedef struct UmiUiWorkspaceLibraryComparison {
    UmiUiWorkspaceLibraryComparisonRow rows[UMI_UI_WORKSPACE_LIBRARY_MAX_COMPARISON_ROWS];
    size_t row_count;
    size_t added_count;
    size_t removed_count;
    size_t changed_count; /* Existing identities with any summary change. */
    uint64_t current_revision;
    uint64_t proposed_revision;
} UmiUiWorkspaceLibraryComparison;

/* Validate all populated rows, identities and exactly one active row in each
 * nonempty input before comparison. Empty snapshots are allowed. Editing
 * returns BUSY, malformed snapshots INVALID_STATE. No heap allocation, I/O or
 * revision mutation occurs. Output stays unchanged on failure and must not
 * overlap either input; inputs may be the same snapshot. */
UmiStatus umi_ui_workspace_library_compare(
    const UmiUiWorkspaceLibrarySnapshot *current,
    const UmiUiWorkspaceLibrarySnapshot *proposed,
    UmiUiWorkspaceLibraryComparison *out_comparison);

#ifdef __cplusplus
}
#endif
#endif
