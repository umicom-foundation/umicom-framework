/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workspace_profile.c
 *
 * PURPOSE:
 *   Implement the thread-safe toolkit-neutral workspace-profile catalogue.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This model stores plain C snapshots. Frontends may display those snapshots,
 * but they must ask the workbench to activate a profile so every frontend and
 * persisted session observes the same state transition.
 */

#include "umicom/ui/workspace_profile.h"

#include <stdlib.h>
#include <string.h>

#include "umicom/platform/threading.h"
#include "ui_internal.h"

struct UmiUiWorkspaceProfileModel {
    UmiUiWorkspaceProfileSnapshot items[UMI_UI_WORKSPACE_PROFILE_MAX];
    size_t count;
    uint64_t revision;
    UmiMutex *mutex;
};

static size_t find_profile(const UmiUiWorkspaceProfileModel *model,
                           const char *profile_id)
{
    size_t index;
    for (index = 0U; index < model->count; ++index) {
        if (strcmp(model->items[index].profile_id, profile_id) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

static int profile_is_valid(const UmiUiWorkspaceProfileSnapshot *profile)
{
    return profile != NULL &&
           umi_ui_id_is_valid(profile->profile_id) &&
           profile->label[0] != '\0' &&
           profile->sidebar_size >= 0 &&
           profile->auxiliary_sidebar_size >= 0 &&
           profile->bottom_panel_size >= 0;
}

UmiStatus umi_ui_workspace_profile_model_create(
    UmiUiWorkspaceProfileModel **out_model)
{
    UmiUiWorkspaceProfileModel *model;
    UmiStatus status;

    if (out_model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_model = NULL;
    model = (UmiUiWorkspaceProfileModel *)calloc(1U, sizeof(*model));
    if (model == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    status = umi_mutex_create(&model->mutex);
    if (status != UMI_STATUS_OK) {
        free(model);
        return status;
    }
    model->revision = 1U;
    *out_model = model;
    return UMI_STATUS_OK;
}

void umi_ui_workspace_profile_model_destroy(UmiUiWorkspaceProfileModel *model)
{
    if (model == NULL) return;
    umi_mutex_destroy(model->mutex);
    free(model);
}

UmiStatus umi_ui_workspace_profile_model_upsert(
    UmiUiWorkspaceProfileModel *model,
    const UmiUiWorkspaceProfileSnapshot *profile)
{
    size_t index;
    if (model == NULL || !profile_is_valid(profile)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)umi_mutex_lock(model->mutex);
    index = find_profile(model, profile->profile_id);
    if (index == SIZE_MAX) {
        if (model->count >= UMI_UI_WORKSPACE_PROFILE_MAX) {
            (void)umi_mutex_unlock(model->mutex);
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        index = model->count++;
    }
    model->items[index] = *profile;
    model->items[index].sidebar_visible = profile->sidebar_visible != 0;
    model->items[index].auxiliary_sidebar_visible =
        profile->auxiliary_sidebar_visible != 0;
    model->items[index].bottom_panel_visible =
        profile->bottom_panel_visible != 0;
    model->items[index].active = profile->active != 0;
    model->items[index].built_in = profile->built_in != 0;
    model->revision = umi_ui_next_revision(model->revision);
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_workspace_profile_model_remove(
    UmiUiWorkspaceProfileModel *model,
    const char *profile_id)
{
    size_t index;
    if (model == NULL || profile_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)umi_mutex_lock(model->mutex);
    index = find_profile(model, profile_id);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    if (index + 1U < model->count) {
        (void)memmove(&model->items[index],
                      &model->items[index + 1U],
                      (model->count - index - 1U) * sizeof(model->items[0]));
    }
    model->count--;
    model->revision = umi_ui_next_revision(model->revision);
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_workspace_profile_model_find(
    const UmiUiWorkspaceProfileModel *model,
    const char *profile_id,
    UmiUiWorkspaceProfileSnapshot *out_profile)
{
    size_t index;
    if (model == NULL || profile_id == NULL || out_profile == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)umi_mutex_lock(model->mutex);
    index = find_profile(model, profile_id);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    *out_profile = model->items[index];
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_workspace_profile_model_at(
    const UmiUiWorkspaceProfileModel *model,
    size_t index,
    UmiUiWorkspaceProfileSnapshot *out_profile)
{
    if (model == NULL || out_profile == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)umi_mutex_lock(model->mutex);
    if (index >= model->count) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    *out_profile = model->items[index];
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_workspace_profile_model_set_active(
    UmiUiWorkspaceProfileModel *model,
    const char *profile_id)
{
    size_t index;
    size_t active_index;
    if (model == NULL || profile_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)umi_mutex_lock(model->mutex);
    active_index = find_profile(model, profile_id);
    if (active_index == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    for (index = 0U; index < model->count; ++index) {
        model->items[index].active = index == active_index;
    }
    model->revision = umi_ui_next_revision(model->revision);
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

size_t umi_ui_workspace_profile_model_count(
    const UmiUiWorkspaceProfileModel *model)
{
    size_t count = 0U;
    if (model != NULL) {
        (void)umi_mutex_lock(model->mutex);
        count = model->count;
        (void)umi_mutex_unlock(model->mutex);
    }
    return count;
}

uint64_t umi_ui_workspace_profile_model_revision(
    const UmiUiWorkspaceProfileModel *model)
{
    uint64_t revision = 0U;
    if (model != NULL) {
        (void)umi_mutex_lock(model->mutex);
        revision = model->revision;
        (void)umi_mutex_unlock(model->mutex);
    }
    return revision;
}
