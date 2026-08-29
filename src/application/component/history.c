/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/history.c
 *
 * PURPOSE:
 *   Apply and reverse individual workspace changes using a bounded history.
 *   When the history fills, the oldest change is discarded predictably.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/component/history.h"

#include <string.h>

void umi_application_component_history_init(UmiApplicationComponentHistory *history) {
  if (history != NULL) {
    (void)memset(history, 0, sizeof(*history));
    history->revision = 1U;
  }
}

UmiStatus umi_application_component_history_record(UmiApplicationComponentHistory *history,
                                                   const UmiApplicationComponentChange *change) {
  if (history == NULL || change == NULL || change->summary[0] == '\0' ||
      change->kind < UMI_APPLICATION_COMPONENT_CHANGE_ADD ||
      change->kind > UMI_APPLICATION_COMPONENT_CHANGE_TITLE ||
      history->count > UMI_APPLICATION_COMPONENT_HISTORY_CAPACITY ||
      history->cursor > history->count)
    return UMI_STATUS_INVALID_ARGUMENT;

  /* A new change after undo starts a new timeline and drops stale redo work. */
  history->count = history->cursor;
  if (history->count == UMI_APPLICATION_COMPONENT_HISTORY_CAPACITY) {
    (void)memmove(&history->changes[0], &history->changes[1],
                  (UMI_APPLICATION_COMPONENT_HISTORY_CAPACITY - 1U) * sizeof(history->changes[0]));
    history->count -= 1U;
    history->cursor -= 1U;
  }
  history->changes[history->cursor] = *change;
  history->cursor += 1U;
  history->count = history->cursor;
  history->revision += 1U;
  return UMI_STATUS_OK;
}

int umi_application_component_history_can_undo(const UmiApplicationComponentHistory *history) {
  return history != NULL && history->count <= UMI_APPLICATION_COMPONENT_HISTORY_CAPACITY &&
         history->cursor <= history->count && history->cursor > 0U;
}

int umi_application_component_history_can_redo(const UmiApplicationComponentHistory *history) {
  return history != NULL && history->count <= UMI_APPLICATION_COMPONENT_HISTORY_CAPACITY &&
         history->cursor < history->count;
}

static UmiStatus apply_change(UmiApplicationComponentWorkspaceDraft *draft,
                              const UmiApplicationComponentChange *change, int forward) {
  switch (change->kind) {
  case UMI_APPLICATION_COMPONENT_CHANGE_ADD:
    return forward
               ? umi_application_component_workspace_draft_insert(draft, change->after_index,
                                                                  &change->after_slot)
               : umi_application_component_workspace_draft_remove(draft, change->after_index, NULL);
  case UMI_APPLICATION_COMPONENT_CHANGE_REMOVE:
    return forward
               ? umi_application_component_workspace_draft_remove(draft, change->before_index, NULL)
               : umi_application_component_workspace_draft_insert(draft, change->before_index,
                                                                  &change->before_slot);
  case UMI_APPLICATION_COMPONENT_CHANGE_MOVE:
    return umi_application_component_workspace_draft_move(
        draft, forward ? change->before_index : change->after_index,
        forward ? change->after_index : change->before_index);
  case UMI_APPLICATION_COMPONENT_CHANGE_VISIBILITY:
    return umi_application_component_workspace_draft_set_visible(
        draft, change->before_index,
        forward ? change->after_slot.visible : change->before_slot.visible);
  case UMI_APPLICATION_COMPONENT_CHANGE_REGION:
    return umi_application_component_workspace_draft_set_region(
        draft, change->before_index,
        forward ? change->after_slot.region : change->before_slot.region);
  case UMI_APPLICATION_COMPONENT_CHANGE_WEIGHT:
    return umi_application_component_workspace_draft_set_weight(
        draft, change->before_index,
        forward ? change->after_slot.weight : change->before_slot.weight);
  case UMI_APPLICATION_COMPONENT_CHANGE_TITLE:
    return umi_application_component_workspace_draft_set_title(
        draft, forward ? change->after_title : change->before_title);
  default:
    return UMI_STATUS_INVALID_ARGUMENT;
  }
}

UmiStatus umi_application_component_history_undo(UmiApplicationComponentHistory *history,
                                                 UmiApplicationComponentWorkspaceDraft *draft) {
  UmiStatus status;
  if (!umi_application_component_history_can_undo(history) || draft == NULL)
    return UMI_STATUS_UNAVAILABLE;
  status = apply_change(draft, &history->changes[history->cursor - 1U], 0);
  if (status == UMI_STATUS_OK) {
    history->cursor -= 1U;
    history->revision += 1U;
  }
  return status;
}

UmiStatus umi_application_component_history_redo(UmiApplicationComponentHistory *history,
                                                 UmiApplicationComponentWorkspaceDraft *draft) {
  UmiStatus status;
  if (!umi_application_component_history_can_redo(history) || draft == NULL)
    return UMI_STATUS_UNAVAILABLE;
  status = apply_change(draft, &history->changes[history->cursor], 1);
  if (status == UMI_STATUS_OK) {
    history->cursor += 1U;
    history->revision += 1U;
  }
  return status;
}
