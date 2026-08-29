/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/draft.c
 *
 * PURPOSE:
 *   Implement bounded workspace editing operations with stable ordering,
 *   lock protection and a monotonically increasing revision number.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/component/draft.h"

#include <string.h>

#include "umicom/base/text.h"

static int valid_region(UmiApplicationComponentRegion region) {
  return region >= UMI_APPLICATION_COMPONENT_REGION_PRIMARY &&
         region <= UMI_APPLICATION_COMPONENT_REGION_FLOATING;
}

static void refresh_order(UmiApplicationComponentWorkspaceDraft *draft) {
  size_t index;
  for (index = 0U; index < draft->slot_count; ++index)
    draft->slots[index].order = (uint32_t)index;
}

static void changed(UmiApplicationComponentWorkspaceDraft *draft) {
  draft->revision += 1U;
  draft->dirty = 1;
}

static UmiStatus copy_slot(UmiApplicationComponentDraftSlot *target,
                           const UmiApplicationComponentRecipeSlot *source, size_t order) {
  UmiStatus status;
  if (target == NULL || source == NULL || source->component_id == NULL ||
      source->instance_id == NULL || source->component_id[0] == '\0' ||
      source->instance_id[0] == '\0' || source->weight == 0U || !valid_region(source->region))
    return UMI_STATUS_INVALID_ARGUMENT;
  (void)memset(target, 0, sizeof(*target));
  status = umi_text_copy(target->component_id, sizeof(target->component_id), source->component_id);
  if (status == UMI_STATUS_OK)
    status = umi_text_copy(target->instance_id, sizeof(target->instance_id), source->instance_id);
  if (status != UMI_STATUS_OK)
    return status;
  target->region = source->region;
  target->order = (uint32_t)order;
  target->weight = source->weight;
  target->visible = source->visible != 0;
  target->locked = source->locked != 0;
  return UMI_STATUS_OK;
}

UmiStatus
umi_application_component_workspace_draft_init(UmiApplicationComponentWorkspaceDraft *draft,
                                               const UmiApplicationComponentRecipe *recipe) {
  size_t index;
  UmiStatus status;
  if (draft == NULL || recipe == NULL || recipe->recipe_id == NULL ||
      recipe->application_id == NULL || recipe->title == NULL || recipe->description == NULL ||
      recipe->experience_profile_id == NULL || recipe->experience_profile_id[0] == '\0' ||
      recipe->slots == NULL || recipe->slot_count == 0U ||
      recipe->slot_count > UMI_APPLICATION_COMPONENT_LAYOUT_CAPACITY)
    return UMI_STATUS_INVALID_ARGUMENT;
  (void)memset(draft, 0, sizeof(*draft));
  status = umi_text_copy(draft->recipe_id, sizeof(draft->recipe_id), recipe->recipe_id);
  if (status == UMI_STATUS_OK)
    status =
        umi_text_copy(draft->application_id, sizeof(draft->application_id), recipe->application_id);
  if (status == UMI_STATUS_OK)
    status = umi_text_copy(draft->experience_profile_id, sizeof(draft->experience_profile_id),
                           recipe->experience_profile_id);
  if (status == UMI_STATUS_OK)
    status = umi_text_copy(draft->title, sizeof(draft->title), recipe->title);
  if (status == UMI_STATUS_OK)
    status = umi_text_copy(draft->description, sizeof(draft->description), recipe->description);
  draft->audience = recipe->audience;
  for (index = 0U; status == UMI_STATUS_OK && index < recipe->slot_count; ++index)
    status = copy_slot(&draft->slots[index], &recipe->slots[index], index);
  if (status != UMI_STATUS_OK) {
    (void)memset(draft, 0, sizeof(*draft));
    return status;
  }
  draft->slot_count = recipe->slot_count;
  draft->revision = 1U;
  return UMI_STATUS_OK;
}

const UmiApplicationComponentDraftSlot *
umi_application_component_workspace_draft_find(const UmiApplicationComponentWorkspaceDraft *draft,
                                               const char *instance_id, size_t *out_index) {
  size_t index;
  if (draft == NULL || instance_id == NULL ||
      draft->slot_count > UMI_APPLICATION_COMPONENT_LAYOUT_CAPACITY)
    return NULL;
  for (index = 0U; index < draft->slot_count; ++index) {
    if (strcmp(draft->slots[index].instance_id, instance_id) == 0) {
      if (out_index != NULL)
        *out_index = index;
      return &draft->slots[index];
    }
  }
  return NULL;
}

UmiStatus
umi_application_component_workspace_draft_insert(UmiApplicationComponentWorkspaceDraft *draft,
                                                 size_t index,
                                                 const UmiApplicationComponentDraftSlot *slot) {
  if (draft == NULL || slot == NULL || index > draft->slot_count ||
      draft->slot_count > UMI_APPLICATION_COMPONENT_LAYOUT_CAPACITY ||
      slot->component_id[0] == '\0' || slot->instance_id[0] == '\0' || slot->weight == 0U ||
      !valid_region(slot->region))
    return UMI_STATUS_INVALID_ARGUMENT;
  if (draft->slot_count >= UMI_APPLICATION_COMPONENT_LAYOUT_CAPACITY)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  if (umi_application_component_workspace_draft_find(draft, slot->instance_id, NULL) != NULL)
    return UMI_STATUS_ALREADY_EXISTS;
  if (index < draft->slot_count)
    (void)memmove(&draft->slots[index + 1U], &draft->slots[index],
                  (draft->slot_count - index) * sizeof(draft->slots[0]));
  draft->slots[index] = *slot;
  draft->slot_count += 1U;
  refresh_order(draft);
  changed(draft);
  return UMI_STATUS_OK;
}

UmiStatus
umi_application_component_workspace_draft_remove(UmiApplicationComponentWorkspaceDraft *draft,
                                                 size_t index,
                                                 UmiApplicationComponentDraftSlot *out_removed) {
  if (draft == NULL || index >= draft->slot_count ||
      draft->slot_count > UMI_APPLICATION_COMPONENT_LAYOUT_CAPACITY)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (draft->slots[index].locked)
    return UMI_STATUS_UNAVAILABLE;
  if (out_removed != NULL)
    *out_removed = draft->slots[index];
  if (index + 1U < draft->slot_count)
    (void)memmove(&draft->slots[index], &draft->slots[index + 1U],
                  (draft->slot_count - index - 1U) * sizeof(draft->slots[0]));
  draft->slot_count -= 1U;
  (void)memset(&draft->slots[draft->slot_count], 0, sizeof(draft->slots[0]));
  refresh_order(draft);
  changed(draft);
  return UMI_STATUS_OK;
}

UmiStatus
umi_application_component_workspace_draft_move(UmiApplicationComponentWorkspaceDraft *draft,
                                               size_t from_index, size_t to_index) {
  UmiApplicationComponentDraftSlot moving;
  if (draft == NULL || from_index >= draft->slot_count || to_index >= draft->slot_count ||
      draft->slot_count > UMI_APPLICATION_COMPONENT_LAYOUT_CAPACITY)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (draft->slots[from_index].locked)
    return UMI_STATUS_UNAVAILABLE;
  if (from_index == to_index)
    return UMI_STATUS_OK;
  moving = draft->slots[from_index];
  if (from_index < to_index)
    (void)memmove(&draft->slots[from_index], &draft->slots[from_index + 1U],
                  (to_index - from_index) * sizeof(draft->slots[0]));
  else
    (void)memmove(&draft->slots[to_index + 1U], &draft->slots[to_index],
                  (from_index - to_index) * sizeof(draft->slots[0]));
  draft->slots[to_index] = moving;
  refresh_order(draft);
  changed(draft);
  return UMI_STATUS_OK;
}

UmiStatus
umi_application_component_workspace_draft_set_visible(UmiApplicationComponentWorkspaceDraft *draft,
                                                      size_t index, int visible) {
  if (draft == NULL || index >= draft->slot_count)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (draft->slots[index].locked)
    return UMI_STATUS_UNAVAILABLE;
  if (draft->slots[index].visible != (visible != 0)) {
    draft->slots[index].visible = visible != 0;
    changed(draft);
  }
  return UMI_STATUS_OK;
}

UmiStatus
umi_application_component_workspace_draft_set_region(UmiApplicationComponentWorkspaceDraft *draft,
                                                     size_t index,
                                                     UmiApplicationComponentRegion region) {
  if (draft == NULL || index >= draft->slot_count || !valid_region(region))
    return UMI_STATUS_INVALID_ARGUMENT;
  if (draft->slots[index].locked)
    return UMI_STATUS_UNAVAILABLE;
  if (draft->slots[index].region != region) {
    draft->slots[index].region = region;
    changed(draft);
  }
  return UMI_STATUS_OK;
}

UmiStatus
umi_application_component_workspace_draft_set_weight(UmiApplicationComponentWorkspaceDraft *draft,
                                                     size_t index, uint32_t weight) {
  if (draft == NULL || index >= draft->slot_count || weight == 0U)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (draft->slots[index].locked)
    return UMI_STATUS_UNAVAILABLE;
  if (draft->slots[index].weight != weight) {
    draft->slots[index].weight = weight;
    changed(draft);
  }
  return UMI_STATUS_OK;
}

UmiStatus
umi_application_component_workspace_draft_set_title(UmiApplicationComponentWorkspaceDraft *draft,
                                                    const char *title) {
  UmiStatus status;
  if (draft == NULL || title == NULL || title[0] == '\0')
    return UMI_STATUS_INVALID_ARGUMENT;
  if (strcmp(draft->title, title) == 0)
    return UMI_STATUS_OK;
  status = umi_text_copy(draft->title, sizeof(draft->title), title);
  if (status == UMI_STATUS_OK)
    changed(draft);
  return status;
}

UmiStatus umi_application_component_workspace_draft_project(
    const UmiApplicationComponentWorkspaceDraft *draft, UmiApplicationComponentLayout *out_layout) {
  size_t index;
  UmiStatus status;
  if (draft == NULL || out_layout == NULL || draft->slot_count == 0U ||
      draft->slot_count > UMI_APPLICATION_COMPONENT_LAYOUT_CAPACITY)
    return UMI_STATUS_INVALID_ARGUMENT;
  status = umi_application_component_layout_init(out_layout, draft->recipe_id, draft->title);
  for (index = 0U; status == UMI_STATUS_OK && index < draft->slot_count; ++index) {
    const UmiApplicationComponentDraftSlot *slot = &draft->slots[index];
    status = umi_application_component_layout_add(out_layout, slot->component_id, slot->instance_id,
                                                  slot->region, slot->weight);
    if (status == UMI_STATUS_OK)
      out_layout->slots[index].visible = slot->visible;
  }
  return status;
}

void umi_application_component_workspace_draft_mark_saved(
    UmiApplicationComponentWorkspaceDraft *draft) {
  if (draft != NULL)
    draft->dirty = 0;
}
