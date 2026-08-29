/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/customiser.c
 *
 * PURPOSE:
 *   Turn low-level draft operations into user-facing editing commands with
 *   meaningful history entries, undo, redo and save-state reporting.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/component/customiser.h"

#include <string.h>

#include "umicom/base/text.h"

static UmiStatus record_change(UmiApplicationComponentCustomiser *customiser,
                               UmiApplicationComponentChange *change, const char *summary) {
  int drops_oldest_change;
  UmiStatus status = umi_text_copy(change->summary, sizeof(change->summary), summary);
  if (customiser->saved_cursor_valid && customiser->saved_cursor > customiser->history.cursor)
    customiser->saved_cursor_valid = 0;
  drops_oldest_change = customiser->history.count == UMI_APPLICATION_COMPONENT_HISTORY_CAPACITY &&
                        customiser->history.cursor == customiser->history.count;
  if (status == UMI_STATUS_OK)
    status = umi_application_component_history_record(&customiser->history, change);
  if (status == UMI_STATUS_OK) {
    if (drops_oldest_change && customiser->saved_cursor_valid) {
      if (customiser->saved_cursor == 0U)
        customiser->saved_cursor_valid = 0;
      else
        customiser->saved_cursor -= 1U;
    }
    customiser->draft.dirty = 1;
  }
  return status;
}

static UmiStatus find_slot(UmiApplicationComponentCustomiser *customiser, const char *instance_id,
                           size_t *out_index) {
  if (customiser == NULL || instance_id == NULL || out_index == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  return umi_application_component_workspace_draft_find(&customiser->draft, instance_id,
                                                        out_index) != NULL
             ? UMI_STATUS_OK
             : UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_application_component_customiser_init(UmiApplicationComponentCustomiser *customiser,
                                                    const UmiApplicationComponentRecipe *recipe) {
  UmiStatus status;
  if (customiser == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  (void)memset(customiser, 0, sizeof(*customiser));
  status = umi_application_component_workspace_draft_init(&customiser->draft, recipe);
  if (status != UMI_STATUS_OK)
    return status;
  umi_application_component_history_init(&customiser->history);
  customiser->saved_revision = customiser->draft.revision;
  customiser->saved_cursor = 0U;
  customiser->saved_cursor_valid = 1;
  return UMI_STATUS_OK;
}

UmiStatus umi_application_component_customiser_add(UmiApplicationComponentCustomiser *customiser,
                                                   const char *component_id,
                                                   const char *instance_id,
                                                   UmiApplicationComponentRegion region,
                                                   uint32_t weight) {
  UmiApplicationComponentChange change;
  UmiStatus status;
  if (customiser == NULL || component_id == NULL || instance_id == NULL ||
      component_id[0] == '\0' || instance_id[0] == '\0')
    return UMI_STATUS_INVALID_ARGUMENT;
  (void)memset(&change, 0, sizeof(change));
  change.kind = UMI_APPLICATION_COMPONENT_CHANGE_ADD;
  change.after_index = customiser->draft.slot_count;
  status = umi_text_copy(change.after_slot.component_id, sizeof(change.after_slot.component_id),
                         component_id);
  if (status == UMI_STATUS_OK)
    status = umi_text_copy(change.after_slot.instance_id, sizeof(change.after_slot.instance_id),
                           instance_id);
  change.after_slot.region = region;
  change.after_slot.order = (uint32_t)change.after_index;
  change.after_slot.weight = weight;
  change.after_slot.visible = 1;
  if (status == UMI_STATUS_OK)
    status = umi_application_component_workspace_draft_insert(
        &customiser->draft, change.after_index, &change.after_slot);
  if (status == UMI_STATUS_OK)
    status = record_change(customiser, &change, "Add component");
  return status;
}

UmiStatus umi_application_component_customiser_remove(UmiApplicationComponentCustomiser *customiser,
                                                      const char *instance_id) {
  UmiApplicationComponentChange change;
  UmiStatus status;
  size_t index;
  status = find_slot(customiser, instance_id, &index);
  if (status != UMI_STATUS_OK)
    return status;
  (void)memset(&change, 0, sizeof(change));
  change.kind = UMI_APPLICATION_COMPONENT_CHANGE_REMOVE;
  change.before_index = index;
  status = umi_application_component_workspace_draft_remove(&customiser->draft, index,
                                                            &change.before_slot);
  if (status == UMI_STATUS_OK)
    status = record_change(customiser, &change, "Remove component");
  return status;
}

UmiStatus umi_application_component_customiser_move(UmiApplicationComponentCustomiser *customiser,
                                                    const char *instance_id, size_t new_index) {
  UmiApplicationComponentChange change;
  UmiStatus status;
  size_t index;
  status = find_slot(customiser, instance_id, &index);
  if (status != UMI_STATUS_OK || new_index >= customiser->draft.slot_count)
    return status != UMI_STATUS_OK ? status : UMI_STATUS_INVALID_ARGUMENT;
  if (index == new_index)
    return UMI_STATUS_OK;
  (void)memset(&change, 0, sizeof(change));
  change.kind = UMI_APPLICATION_COMPONENT_CHANGE_MOVE;
  change.before_index = index;
  change.after_index = new_index;
  change.before_slot = customiser->draft.slots[index];
  status = umi_application_component_workspace_draft_move(&customiser->draft, index, new_index);
  if (status == UMI_STATUS_OK) {
    change.after_slot = customiser->draft.slots[new_index];
    status = record_change(customiser, &change, "Move component");
  }
  return status;
}

static UmiStatus record_slot_change(UmiApplicationComponentCustomiser *customiser, size_t index,
                                    UmiApplicationComponentChangeKind kind,
                                    const UmiApplicationComponentDraftSlot *before,
                                    const char *summary) {
  UmiApplicationComponentChange change;
  (void)memset(&change, 0, sizeof(change));
  change.kind = kind;
  change.before_index = index;
  change.after_index = index;
  change.before_slot = *before;
  change.after_slot = customiser->draft.slots[index];
  return record_change(customiser, &change, summary);
}

UmiStatus
umi_application_component_customiser_set_visible(UmiApplicationComponentCustomiser *customiser,
                                                 const char *instance_id, int visible) {
  UmiApplicationComponentDraftSlot before;
  size_t index;
  UmiStatus status = find_slot(customiser, instance_id, &index);
  if (status != UMI_STATUS_OK)
    return status;
  before = customiser->draft.slots[index];
  if (before.visible == (visible != 0))
    return UMI_STATUS_OK;
  status =
      umi_application_component_workspace_draft_set_visible(&customiser->draft, index, visible);
  return status == UMI_STATUS_OK
             ? record_slot_change(customiser, index, UMI_APPLICATION_COMPONENT_CHANGE_VISIBILITY,
                                  &before, "Change component visibility")
             : status;
}

UmiStatus
umi_application_component_customiser_set_region(UmiApplicationComponentCustomiser *customiser,
                                                const char *instance_id,
                                                UmiApplicationComponentRegion region) {
  UmiApplicationComponentDraftSlot before;
  size_t index;
  UmiStatus status = find_slot(customiser, instance_id, &index);
  if (status != UMI_STATUS_OK)
    return status;
  before = customiser->draft.slots[index];
  if (before.region == region)
    return UMI_STATUS_OK;
  status = umi_application_component_workspace_draft_set_region(&customiser->draft, index, region);
  return status == UMI_STATUS_OK
             ? record_slot_change(customiser, index, UMI_APPLICATION_COMPONENT_CHANGE_REGION,
                                  &before, "Move component to another region")
             : status;
}

UmiStatus
umi_application_component_customiser_set_weight(UmiApplicationComponentCustomiser *customiser,
                                                const char *instance_id, uint32_t weight) {
  UmiApplicationComponentDraftSlot before;
  size_t index;
  UmiStatus status = find_slot(customiser, instance_id, &index);
  if (status != UMI_STATUS_OK)
    return status;
  before = customiser->draft.slots[index];
  if (before.weight == weight)
    return UMI_STATUS_OK;
  status = umi_application_component_workspace_draft_set_weight(&customiser->draft, index, weight);
  return status == UMI_STATUS_OK
             ? record_slot_change(customiser, index, UMI_APPLICATION_COMPONENT_CHANGE_WEIGHT,
                                  &before, "Resize component")
             : status;
}

UmiStatus
umi_application_component_customiser_set_title(UmiApplicationComponentCustomiser *customiser,
                                               const char *title) {
  UmiApplicationComponentChange change;
  UmiStatus status;
  if (customiser == NULL || title == NULL || title[0] == '\0')
    return UMI_STATUS_INVALID_ARGUMENT;
  if (strcmp(customiser->draft.title, title) == 0)
    return UMI_STATUS_OK;
  (void)memset(&change, 0, sizeof(change));
  change.kind = UMI_APPLICATION_COMPONENT_CHANGE_TITLE;
  status = umi_text_copy(change.before_title, sizeof(change.before_title), customiser->draft.title);
  if (status == UMI_STATUS_OK)
    status = umi_text_copy(change.after_title, sizeof(change.after_title), title);
  if (status == UMI_STATUS_OK)
    status = umi_application_component_workspace_draft_set_title(&customiser->draft, title);
  if (status == UMI_STATUS_OK)
    status = record_change(customiser, &change, "Rename workspace");
  return status;
}

static void refresh_dirty(UmiApplicationComponentCustomiser *customiser) {
  customiser->draft.dirty =
      !customiser->saved_cursor_valid || customiser->history.cursor != customiser->saved_cursor;
}

UmiStatus umi_application_component_customiser_undo(UmiApplicationComponentCustomiser *customiser) {
  UmiStatus status;
  if (customiser == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  status = umi_application_component_history_undo(&customiser->history, &customiser->draft);
  if (status == UMI_STATUS_OK)
    refresh_dirty(customiser);
  return status;
}

UmiStatus umi_application_component_customiser_redo(UmiApplicationComponentCustomiser *customiser) {
  UmiStatus status;
  if (customiser == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  status = umi_application_component_history_redo(&customiser->history, &customiser->draft);
  if (status == UMI_STATUS_OK)
    refresh_dirty(customiser);
  return status;
}

void umi_application_component_customiser_mark_saved(
    UmiApplicationComponentCustomiser *customiser) {
  if (customiser != NULL) {
    customiser->saved_revision = customiser->draft.revision;
    customiser->saved_cursor = customiser->history.cursor;
    customiser->saved_cursor_valid = 1;
    umi_application_component_workspace_draft_mark_saved(&customiser->draft);
  }
}

UmiStatus umi_application_component_customiser_snapshot(
    const UmiApplicationComponentCustomiser *customiser,
    UmiApplicationComponentCustomiserSnapshot *out_snapshot) {
  size_t index;
  if (customiser == NULL || out_snapshot == NULL ||
      customiser->draft.slot_count > UMI_APPLICATION_COMPONENT_LAYOUT_CAPACITY ||
      customiser->history.cursor > customiser->history.count)
    return UMI_STATUS_INVALID_ARGUMENT;
  (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
  out_snapshot->recipe_id = customiser->draft.recipe_id;
  out_snapshot->application_id = customiser->draft.application_id;
  out_snapshot->title = customiser->draft.title;
  out_snapshot->slot_count = customiser->draft.slot_count;
  for (index = 0U; index < customiser->draft.slot_count; ++index)
    out_snapshot->visible_count += customiser->draft.slots[index].visible != 0;
  out_snapshot->undo_count = customiser->history.cursor;
  out_snapshot->redo_count = customiser->history.count - customiser->history.cursor;
  out_snapshot->revision = customiser->draft.revision;
  out_snapshot->dirty = customiser->draft.dirty;
  return UMI_STATUS_OK;
}
