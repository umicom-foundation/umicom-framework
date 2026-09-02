/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/collaboration.c
 *
 * PURPOSE:
 *   Implement deterministic collaborator projection for status, canvas cursors
 *   and conflict surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/collaboration.h"
#include "internal.h"


/*
 * Initialise workbench designer collaboration from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_designer_collaboration_init(
    UmiWorkbenchDesignerCollaborationModel *model)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return;
    (void)memset(model, 0, sizeof(*model));
}

/*
 * Find workbench designer collaboration while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiWorkbenchDesignerCollaborator *umi_workbench_designer_collaboration_find(
    const UmiWorkbenchDesignerCollaborationModel *model,
    const char *user_id,
    const char *client_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || user_id == NULL || client_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(model->collaborators[index].user_id, user_id) == 0 &&
            strcmp(model->collaborators[index].client_id, client_id) == 0) {
            return &model->collaborators[index];
        }
    }
    return NULL;
}

/*
 * Provide the workbench designer collaboration upsert operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_designer_collaboration_upsert(
    UmiWorkbenchDesignerCollaborationModel *model,
    const UmiWorkbenchDesignerCollaborator *collaborator)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || collaborator == NULL ||
        collaborator->user_id[0] == '\0' || collaborator->client_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(model->collaborators[index].user_id, collaborator->user_id) == 0 &&
            strcmp(model->collaborators[index].client_id, collaborator->client_id) == 0) {
            model->collaborators[index] = *collaborator;
            model->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (model->count >= UMI_WORKBENCH_DESIGNER_MAX_COLLABORATORS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    model->collaborators[model->count++] = *collaborator;
    model->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Remove workbench designer collaboration while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_workbench_designer_collaboration_remove(
    UmiWorkbenchDesignerCollaborationModel *model,
    const char *user_id,
    const char *client_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || user_id == NULL || client_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(model->collaborators[index].user_id, user_id) == 0 &&
            strcmp(model->collaborators[index].client_id, client_id) == 0) {
            size_t move_index;
            /* Visit each bounded item once so every record receives the same rule. */
            for (move_index = index + 1U; move_index < model->count; ++move_index) {
                model->collaborators[move_index - 1U] = model->collaborators[move_index];
            }
            model->count -= 1U;
            model->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Return the number of records represented by workbench designer collaboration editing
 * without changing their state.
 */
size_t umi_workbench_designer_collaboration_editing_count(
    const UmiWorkbenchDesignerCollaborationModel *model)
{
    size_t index;
    size_t count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (model->collaborators[index].state ==
            UMI_WORKBENCH_DESIGNER_COLLABORATOR_EDITING) count += 1U;
    }
    return count;
}
