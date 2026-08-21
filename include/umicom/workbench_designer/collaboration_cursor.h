/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/collaboration_cursor.h
 *
 * PURPOSE:
 *   Project remote collaborator cursors, selections and viewport intent into
 *   bounded frontend-neutral state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_COLLABORATION_CURSOR_H
#define UMICOM_WORKBENCH_DESIGNER_COLLABORATION_CURSOR_H

#include "umicom/workbench_designer/selection.h"

#ifdef __cplusplus
extern "C" {
#endif


#define UMI_WORKBENCH_DESIGNER_MAX_REMOTE_CURSORS 32U

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

typedef struct UmiWorkbenchDesignerRemoteCursorModel {
    UmiWorkbenchDesignerRemoteCursor cursors[UMI_WORKBENCH_DESIGNER_MAX_REMOTE_CURSORS];
    size_t count;
    uint64_t revision;
} UmiWorkbenchDesignerRemoteCursorModel;

void umi_workbench_designer_remote_cursor_model_init(UmiWorkbenchDesignerRemoteCursorModel *model);
UmiStatus umi_workbench_designer_remote_cursor_upsert(UmiWorkbenchDesignerRemoteCursorModel *model, const UmiWorkbenchDesignerRemoteCursor *cursor);
UmiStatus umi_workbench_designer_remote_cursor_remove(UmiWorkbenchDesignerRemoteCursorModel *model, const char *user_id, const char *client_id);
size_t umi_workbench_designer_remote_cursor_prune(UmiWorkbenchDesignerRemoteCursorModel *model, uint64_t now_ms, uint64_t timeout_ms);
const UmiWorkbenchDesignerRemoteCursor *umi_workbench_designer_remote_cursor_find(const UmiWorkbenchDesignerRemoteCursorModel *model, const char *user_id, const char *client_id);

#ifdef __cplusplus
}
#endif

#endif
