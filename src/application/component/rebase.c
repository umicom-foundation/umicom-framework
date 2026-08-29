/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/rebase.c
 *
 * PURPOSE:
 *   Rebase a customised workspace onto a new Framework recipe while preserving
 *   matching placement choices and reporting what changed.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/component/rebase.h"

#include <string.h>

#include "umicom/base/text.h"

static int slot_matches(const UmiApplicationComponentDraftSlot *left,
                        const UmiApplicationComponentDraftSlot *right) {
  return strcmp(left->component_id, right->component_id) == 0 &&
         strcmp(left->instance_id, right->instance_id) == 0;
}

static size_t find_matching_slot(const UmiApplicationComponentWorkspaceDraft *draft,
                                 const UmiApplicationComponentDraftSlot *slot) {
  size_t index;
  for (index = 0U; index < draft->slot_count; ++index) {
    if (slot_matches(&draft->slots[index], slot))
      return index;
  }
  return draft->slot_count;
}

UmiStatus umi_application_component_workspace_rebase(
    UmiApplicationComponentWorkspaceDraft *draft, const UmiApplicationComponentRecipe *new_recipe,
    int keep_removed_components, UmiApplicationComponentRebaseReport *out_report) {
  UmiApplicationComponentWorkspaceDraft rebased;
  UmiApplicationComponentRebaseReport report;
  unsigned char matched[UMI_APPLICATION_COMPONENT_LAYOUT_CAPACITY];
  size_t index;
  UmiStatus status;
  if (draft == NULL || new_recipe == NULL || out_report == NULL ||
      draft->slot_count > UMI_APPLICATION_COMPONENT_LAYOUT_CAPACITY)
    return UMI_STATUS_INVALID_ARGUMENT;
  (void)memset(&report, 0, sizeof(report));
  (void)memset(matched, 0, sizeof(matched));
  report.previous_revision = draft->revision;
  status = umi_application_component_workspace_draft_init(&rebased, new_recipe);
  if (status != UMI_STATUS_OK)
    return status;

  /* A user-supplied title is personal workspace state, so keep it on upgrade. */
  status = umi_text_copy(rebased.title, sizeof(rebased.title), draft->title);
  if (status != UMI_STATUS_OK)
    return status;
  for (index = 0U; index < rebased.slot_count; ++index) {
    size_t old_index = find_matching_slot(draft, &rebased.slots[index]);
    if (old_index < draft->slot_count) {
      const UmiApplicationComponentDraftSlot *old_slot = &draft->slots[old_index];
      rebased.slots[index].region = old_slot->region;
      rebased.slots[index].weight = old_slot->weight;
      rebased.slots[index].visible = old_slot->visible;
      matched[old_index] = 1U;
      report.preserved_count += 1U;
    } else {
      report.added_count += 1U;
    }
  }
  for (index = 0U; index < draft->slot_count; ++index) {
    if (matched[index])
      continue;
    report.removed_count += 1U;
    if (keep_removed_components) {
      if (rebased.slot_count >= UMI_APPLICATION_COMPONENT_LAYOUT_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
      rebased.slots[rebased.slot_count] = draft->slots[index];
      rebased.slots[rebased.slot_count].order = (uint32_t)rebased.slot_count;
      rebased.slot_count += 1U;
    }
  }
  rebased.revision = draft->revision + 1U;
  rebased.dirty = 1;
  report.result_revision = rebased.revision;
  *draft = rebased;
  *out_report = report;
  return UMI_STATUS_OK;
}
