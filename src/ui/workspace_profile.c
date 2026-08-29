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

#include "umicom/ui/internal/workspace_profile_owner.h"

#include <stdlib.h>
#include <string.h>

#include "ui_internal.h"
#include "umicom/platform/threading.h"

struct UmiUiWorkspaceProfileModel {
  UmiUiWorkspaceProfileSnapshot items[UMI_UI_WORKSPACE_PROFILE_MAX];
  size_t count;
  uint64_t revision;
  UmiMutex *mutex;
};

static size_t find_profile(const UmiUiWorkspaceProfileModel *model, const char *profile_id) {
  size_t index;
  for (index = 0U; index < model->count; ++index) {
    if (strcmp(model->items[index].profile_id, profile_id) == 0) {
      return index;
    }
  }
  return SIZE_MAX;
}

static int profile_is_valid(const UmiUiWorkspaceProfileSnapshot *profile) {
  size_t index;
  if (profile == NULL || !umi_ui_id_is_valid(profile->profile_id) || profile->label[0] == '\0' ||
      profile->sidebar_size < 0 || profile->auxiliary_sidebar_size < 0 ||
      profile->bottom_panel_size < 0 || profile->editor_split_mode < UMI_UI_EDITOR_SPLIT_SINGLE ||
      profile->editor_split_mode > UMI_UI_EDITOR_SPLIT_ROWS ||
      (profile->editor_split_ratio != 0 &&
       (profile->editor_split_ratio < UMI_UI_EDITOR_SPLIT_RATIO_MIN ||
        profile->editor_split_ratio > UMI_UI_EDITOR_SPLIT_RATIO_MAX)) ||
      profile->pane_count > UMI_UI_WORKSPACE_PROFILE_MAX_PANES) {
    return 0;
  }
  for (index = 0U; index < profile->pane_count; ++index) {
    const UmiUiWorkspacePanePlacement *pane = &profile->panes[index];
    if (!umi_ui_id_is_valid(pane->pane_id) ||
        (pane->placement != UMI_UI_PLACEMENT_LEFT && pane->placement != UMI_UI_PLACEMENT_RIGHT &&
         pane->placement != UMI_UI_PLACEMENT_BOTTOM && pane->placement != UMI_UI_PLACEMENT_TOP &&
         pane->placement != UMI_UI_PLACEMENT_CENTRE)) {
      return 0;
    }
  }
  return 1;
}

/* Normalize every stored snapshot through one path so ordinary user-profile
 * updates and privileged built-in refreshes retain identical invariants. */
static void store_profile(UmiUiWorkspaceProfileModel *model, size_t index,
                          const UmiUiWorkspaceProfileSnapshot *profile, int active) {
  size_t pane_index;
  model->items[index] = *profile;
  model->items[index].sidebar_visible = profile->sidebar_visible != 0;
  model->items[index].auxiliary_sidebar_visible = profile->auxiliary_sidebar_visible != 0;
  model->items[index].bottom_panel_visible = profile->bottom_panel_visible != 0;
  if (model->items[index].editor_split_ratio == 0) {
    model->items[index].editor_split_ratio = UMI_UI_EDITOR_SPLIT_RATIO_DEFAULT;
  }
  model->items[index].active = active != 0;
  model->items[index].built_in = profile->built_in != 0;
  model->items[index].locked = profile->built_in || profile->locked != 0;
  for (pane_index = 0U; pane_index < model->items[index].pane_count; ++pane_index) {
    model->items[index].panes[pane_index].visible =
        model->items[index].panes[pane_index].visible != 0;
  }
}

UmiStatus umi_ui_workspace_profile_model_create(UmiUiWorkspaceProfileModel **out_model) {
  UmiUiWorkspaceProfileModel *model;
  UmiStatus status;

  if (out_model == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out_model = NULL;
  model = (UmiUiWorkspaceProfileModel *)calloc(1U, sizeof(*model));
  if (model == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;

  status = umi_mutex_create(&model->mutex);
  if (status != UMI_STATUS_OK) {
    free(model);
    return status;
  }
  model->revision = 1U;
  *out_model = model;
  return UMI_STATUS_OK;
}

void umi_ui_workspace_profile_model_destroy(UmiUiWorkspaceProfileModel *model) {
  if (model == NULL)
    return;
  umi_mutex_destroy(model->mutex);
  free(model);
}

UmiStatus umi_ui_workspace_profile_model_upsert(UmiUiWorkspaceProfileModel *model,
                                                const UmiUiWorkspaceProfileSnapshot *profile) {
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
  } else if (model->items[index].built_in || model->items[index].locked) {
    /* Protected layouts must be changed through explicit Framework
     * operations. A generic upsert must never overwrite a built-in or
     * user-locked profile by accident. */
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_PERMISSION_DENIED;
  }
  store_profile(model, index, profile, profile->active);
  model->revision = umi_ui_next_revision(model->revision);
  (void)umi_mutex_unlock(model->mutex);
  return UMI_STATUS_OK;
}

UmiStatus
umi_ui_workspace_profile_model_install_built_in(UmiUiWorkspaceProfileModel *model,
                                                const UmiUiWorkspaceProfileSnapshot *profile) {
  size_t index;
  int active;
  if (model == NULL || !profile_is_valid(profile) || !profile->built_in) {
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
    active = profile->active;
  } else {
    /* A Framework refresh may replace only a profile it already owns. */
    if (!model->items[index].built_in) {
      (void)umi_mutex_unlock(model->mutex);
      return UMI_STATUS_PERMISSION_DENIED;
    }
    active = model->items[index].active;
  }
  store_profile(model, index, profile, active);
  model->revision = umi_ui_next_revision(model->revision);
  (void)umi_mutex_unlock(model->mutex);
  return UMI_STATUS_OK;
}

UmiStatus umi_ui_workspace_profile_model_remove(UmiUiWorkspaceProfileModel *model,
                                                const char *profile_id) {
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
  if (model->items[index].built_in || model->items[index].locked) {
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_PERMISSION_DENIED;
  }
  if (model->items[index].active) {
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_INVALID_STATE;
  }
  if (index + 1U < model->count) {
    (void)memmove(&model->items[index], &model->items[index + 1U],
                  (model->count - index - 1U) * sizeof(model->items[0]));
  }
  model->count--;
  model->revision = umi_ui_next_revision(model->revision);
  (void)umi_mutex_unlock(model->mutex);
  return UMI_STATUS_OK;
}

UmiStatus umi_ui_workspace_profile_model_find(const UmiUiWorkspaceProfileModel *model,
                                              const char *profile_id,
                                              UmiUiWorkspaceProfileSnapshot *out_profile) {
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

UmiStatus umi_ui_workspace_profile_model_at(const UmiUiWorkspaceProfileModel *model, size_t index,
                                            UmiUiWorkspaceProfileSnapshot *out_profile) {
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

UmiStatus umi_ui_workspace_profile_model_set_active(UmiUiWorkspaceProfileModel *model,
                                                    const char *profile_id) {
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

UmiStatus umi_ui_workspace_profile_model_rename(UmiUiWorkspaceProfileModel *model,
                                                const char *profile_id, const char *label,
                                                const char *description) {
  size_t index;
  size_t label_length;
  size_t description_length;

  if (model == NULL || profile_id == NULL || label == NULL || description == NULL ||
      label[0] == '\0') {
    return UMI_STATUS_INVALID_ARGUMENT;
  }
  label_length = strlen(label);
  description_length = strlen(description);
  if (label_length >= UMI_UI_TEXT_CAPACITY || description_length >= UMI_UI_DESCRIPTION_CAPACITY) {
    return UMI_STATUS_CAPACITY_EXCEEDED;
  }

  (void)umi_mutex_lock(model->mutex);
  index = find_profile(model, profile_id);
  if (index == SIZE_MAX) {
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_NOT_FOUND;
  }
  if (model->items[index].built_in || model->items[index].locked) {
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_PERMISSION_DENIED;
  }
  (void)memcpy(model->items[index].label, label, label_length + 1U);
  (void)memcpy(model->items[index].description, description, description_length + 1U);
  model->revision = umi_ui_next_revision(model->revision);
  (void)umi_mutex_unlock(model->mutex);
  return UMI_STATUS_OK;
}

UmiStatus umi_ui_workspace_profile_model_set_locked(UmiUiWorkspaceProfileModel *model,
                                                    const char *profile_id, int locked) {
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
  if (model->items[index].built_in) {
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_PERMISSION_DENIED;
  }
  model->items[index].locked = locked != 0;
  model->revision = umi_ui_next_revision(model->revision);
  (void)umi_mutex_unlock(model->mutex);
  return UMI_STATUS_OK;
}

size_t umi_ui_workspace_profile_model_count(const UmiUiWorkspaceProfileModel *model) {
  size_t count = 0U;
  if (model != NULL) {
    (void)umi_mutex_lock(model->mutex);
    count = model->count;
    (void)umi_mutex_unlock(model->mutex);
  }
  return count;
}

uint64_t umi_ui_workspace_profile_model_revision(const UmiUiWorkspaceProfileModel *model) {
  uint64_t revision = 0U;
  if (model != NULL) {
    (void)umi_mutex_lock(model->mutex);
    revision = model->revision;
    (void)umi_mutex_unlock(model->mutex);
  }
  return revision;
}
