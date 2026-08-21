/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/collaboration.h
 *
 * PURPOSE:
 *   Track collaborators, active nodes and edit intent for shared layout
 *   sessions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_COLLABORATION_H
#define UMICOM_WORKBENCH_DESIGNER_COLLABORATION_H

#include "umicom/workbench_designer/types.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum UmiWorkbenchDesignerCollaborationState {
    UMI_WORKBENCH_DESIGNER_COLLABORATOR_VIEWING = 1,
    UMI_WORKBENCH_DESIGNER_COLLABORATOR_EDITING = 2,
    UMI_WORKBENCH_DESIGNER_COLLABORATOR_IDLE = 3,
    UMI_WORKBENCH_DESIGNER_COLLABORATOR_OFFLINE = 4,
    UMI_WORKBENCH_DESIGNER_COLLABORATOR_CONFLICT = 5
} UmiWorkbenchDesignerCollaborationState;

typedef struct UmiWorkbenchDesignerCollaborator {
    char user_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char client_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char display_name[UMI_WORKBENCH_DESIGNER_LABEL_CAPACITY];
    char active_node_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char context_group_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    UmiWorkbenchDesignerCollaborationState state;
    uint32_t colour_index;
    uint64_t connected_at_ms;
    uint64_t last_activity_ms;
    bool local_user;
} UmiWorkbenchDesignerCollaborator;

typedef struct UmiWorkbenchDesignerCollaborationModel {
    UmiWorkbenchDesignerCollaborator collaborators[UMI_WORKBENCH_DESIGNER_MAX_COLLABORATORS];
    size_t count;
    uint64_t revision;
} UmiWorkbenchDesignerCollaborationModel;

void umi_workbench_designer_collaboration_init(UmiWorkbenchDesignerCollaborationModel *model);
UmiStatus umi_workbench_designer_collaboration_upsert(UmiWorkbenchDesignerCollaborationModel *model, const UmiWorkbenchDesignerCollaborator *collaborator);
UmiStatus umi_workbench_designer_collaboration_remove(UmiWorkbenchDesignerCollaborationModel *model, const char *user_id, const char *client_id);
const UmiWorkbenchDesignerCollaborator *umi_workbench_designer_collaboration_find(const UmiWorkbenchDesignerCollaborationModel *model, const char *user_id, const char *client_id);
size_t umi_workbench_designer_collaboration_editing_count(const UmiWorkbenchDesignerCollaborationModel *model);

#ifdef __cplusplus
}
#endif

#endif
