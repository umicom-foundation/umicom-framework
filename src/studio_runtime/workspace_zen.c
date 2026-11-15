/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/workspace_zen.c
 *
 * PURPOSE:
 *   Resolve and toggle existing Zen layouts without copying or replacing arrangements.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/workspace_zen.h"
#include <string.h>

static const char ZEN_ID[] = UMI_STUDIO_RUNTIME_WORKSPACE_PREFIX "zen";
static const char DEFAULT_ID[] = UMI_STUDIO_RUNTIME_WORKSPACE_PREFIX "default";

/* Layout IDs may originate in restored records. Validate fixed strings before
 * any comparison, and refuse ambiguous duplicate identities rather than
 * choosing whichever record happens to appear first. */
static UmiStatus ZenTarget(const UmiUiWorkspaceCustomisation *model,
    const UmiStudioWorkspaceZenState *state,
    char target[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY])
{
    if (model == NULL || state == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (model->layout_count == 0U || model->layout_count > UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS ||
        memchr(model->active_layout_id, '\0', sizeof model->active_layout_id) == NULL ||
        memchr(state->return_layout_id, '\0', sizeof state->return_layout_id) == NULL)
        return UMI_STATUS_INVALID_STATE;
    if (model->edit_active) return UMI_STATUS_BUSY;
    if (model->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    bool active_found = false, return_found = false;
    for (size_t i = 0U; i < model->layout_count; ++i) {
        const char *id = model->layouts[i].layout_id;
        if (id[0] == '\0' || memchr(id, '\0', sizeof model->layouts[i].layout_id) == NULL)
            return UMI_STATUS_INVALID_STATE;
        for (size_t j = 0U; j < i; ++j)
            if (strcmp(id, model->layouts[j].layout_id) == 0) return UMI_STATUS_INVALID_STATE;
        if (strcmp(id, model->active_layout_id) == 0) active_found = true;
        if (strcmp(id, state->return_layout_id) == 0 && strcmp(id, ZEN_ID) != 0)
            return_found = true;
    }
    if (!active_found) return UMI_STATUS_INVALID_STATE;
    const char *wanted = strcmp(model->active_layout_id, ZEN_ID) == 0
        ? (return_found ? state->return_layout_id : DEFAULT_ID) : ZEN_ID;
    for (size_t i = 0U; i < model->layout_count; ++i) {
        if (strcmp(wanted, model->layouts[i].layout_id) != 0) continue;
        (void)memcpy(target, wanted, strlen(wanted) + 1U);
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_NOT_FOUND;
}

bool umi_studio_runtime_workspace_zen_can_toggle(
    const UmiUiWorkspaceCustomisation *model, const UmiStudioWorkspaceZenState *state)
{
    char target[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    return ZenTarget(model, state, target) == UMI_STATUS_OK;
}

UmiStatus umi_studio_runtime_workspace_toggle_zen(
    UmiUiWorkspaceCustomisation *model, UmiStudioWorkspaceZenState *state)
{
    char target[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    UmiStatus status = ZenTarget(model, state, target);
    if (status != UMI_STATUS_OK) return status;
    UmiStudioWorkspaceZenState next = {0};
    if (strcmp(target, ZEN_ID) == 0)
        (void)memcpy(next.return_layout_id, model->active_layout_id,
            strlen(model->active_layout_id) + 1U);
    /* The existing activation owner changes only selected identity/revision.
     * The bounded, existing target has already passed every refusal above. */
    status = umi_studio_runtime_workspace_select(model, target);
    if (status == UMI_STATUS_OK) *state = next;
    return status;
}
