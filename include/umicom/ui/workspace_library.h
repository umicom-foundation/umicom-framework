/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workspace_library.h
 * PURPOSE: Copied named-layout views and atomic library actions over the
 * existing workspace customisation owner. No second registry or persistence.
 * AUTHOR: Sammy Hegab, Umicom Foundation
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

/* Read copied rows in stable model order, including during editing. Output is
 * unchanged on error and must not overlap the model. No pointers are retained. */
UmiStatus umi_ui_workspace_library_snapshot(
    const UmiUiWorkspaceCustomisation *customisation,
    const UmiUiWorkspaceLibraryPolicy *policy,
    UmiUiWorkspaceLibrarySnapshot *out_snapshot);

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
#ifdef __cplusplus
}
#endif
#endif
