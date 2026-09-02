/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/collaboration_cursor.h
 *
 * PURPOSE:
 *   Project remote collaborator cursors, selections and viewport intent into
 *   bounded frontend-neutral state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_COLLABORATION_CURSOR_H
#define UMICOM_WORKBENCH_DESIGNER_COLLABORATION_CURSOR_H

#include "umicom/workbench_designer/selection.h"

#ifdef __cplusplus
extern "C" {
#endif


#define UMI_WORKBENCH_DESIGNER_MAX_REMOTE_CURSORS 32U

/**
 * Represent the workbench designer remote cursor data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerRemoteCursor {
    char user_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char client_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char display_name[UMI_WORKBENCH_DESIGNER_LABEL_CAPACITY];
    UmiWorkbenchDesignerPoint world_position;
    UmiWorkbenchDesignerSelection selection;
    uint32_t colour_index;
    uint64_t sequence;
    uint64_t last_update_ms;
    bool visible;
} UmiWorkbenchDesignerRemoteCursor;

/**
 * Represent the workbench designer remote cursor model data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchDesignerRemoteCursorModel {
    UmiWorkbenchDesignerRemoteCursor cursors[UMI_WORKBENCH_DESIGNER_MAX_REMOTE_CURSORS];
    size_t count;
    uint64_t revision;
} UmiWorkbenchDesignerRemoteCursorModel;

/**
 * Initialise workbench designer remote cursor model from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_designer_remote_cursor_model_init(UmiWorkbenchDesignerRemoteCursorModel *model);
/**
 * Provide the workbench designer remote cursor upsert operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_designer_remote_cursor_upsert(UmiWorkbenchDesignerRemoteCursorModel *model, const UmiWorkbenchDesignerRemoteCursor *cursor);
/**
 * Remove workbench designer remote cursor while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_workbench_designer_remote_cursor_remove(UmiWorkbenchDesignerRemoteCursorModel *model, const char *user_id, const char *client_id);
/**
 * Provide the workbench designer remote cursor prune operation used by this module and its
 * client applications.
 */
size_t umi_workbench_designer_remote_cursor_prune(UmiWorkbenchDesignerRemoteCursorModel *model, uint64_t now_ms, uint64_t timeout_ms);
/**
 * Find workbench designer remote cursor while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiWorkbenchDesignerRemoteCursor *umi_workbench_designer_remote_cursor_find(const UmiWorkbenchDesignerRemoteCursorModel *model, const char *user_id, const char *client_id);

#ifdef __cplusplus
}
#endif

#endif
