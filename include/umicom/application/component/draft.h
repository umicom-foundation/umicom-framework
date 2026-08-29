/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/component/draft.h
 *
 * PURPOSE:
 *   Hold an editable copy of a workspace recipe and provide small, predictable
 *   operations for adding, removing, moving and configuring component slots.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_COMPONENT_DRAFT_H
#define UMICOM_APPLICATION_COMPONENT_DRAFT_H

#include "umicom/application/component/recipe.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationComponentDraftSlot {
  char component_id[UMI_APPLICATION_COMPONENT_ID_CAPACITY];
  char instance_id[UMI_APPLICATION_COMPONENT_ID_CAPACITY];
  UmiApplicationComponentRegion region;
  uint32_t order;
  uint32_t weight;
  int visible;
  int locked;
} UmiApplicationComponentDraftSlot;

typedef struct UmiApplicationComponentWorkspaceDraft {
  char recipe_id[UMI_APPLICATION_COMPONENT_ID_CAPACITY];
  char application_id[UMI_APPLICATION_COMPONENT_APPLICATION_ID_CAPACITY];
  char experience_profile_id[UMI_APPLICATION_COMPONENT_ID_CAPACITY];
  char title[UMI_APPLICATION_COMPONENT_TITLE_CAPACITY];
  char description[UMI_APPLICATION_COMPONENT_DESCRIPTION_CAPACITY];
  UmiApplicationComponentRecipeAudience audience;
  UmiApplicationComponentDraftSlot slots[UMI_APPLICATION_COMPONENT_LAYOUT_CAPACITY];
  size_t slot_count;
  uint64_t revision;
  int dirty;
} UmiApplicationComponentWorkspaceDraft;

UmiStatus
umi_application_component_workspace_draft_init(UmiApplicationComponentWorkspaceDraft *draft,
                                               const UmiApplicationComponentRecipe *recipe);
const UmiApplicationComponentDraftSlot *
umi_application_component_workspace_draft_find(const UmiApplicationComponentWorkspaceDraft *draft,
                                               const char *instance_id, size_t *out_index);
UmiStatus
umi_application_component_workspace_draft_insert(UmiApplicationComponentWorkspaceDraft *draft,
                                                 size_t index,
                                                 const UmiApplicationComponentDraftSlot *slot);
UmiStatus
umi_application_component_workspace_draft_remove(UmiApplicationComponentWorkspaceDraft *draft,
                                                 size_t index,
                                                 UmiApplicationComponentDraftSlot *out_removed);
UmiStatus
umi_application_component_workspace_draft_move(UmiApplicationComponentWorkspaceDraft *draft,
                                               size_t from_index, size_t to_index);
UmiStatus
umi_application_component_workspace_draft_set_visible(UmiApplicationComponentWorkspaceDraft *draft,
                                                      size_t index, int visible);
UmiStatus
umi_application_component_workspace_draft_set_region(UmiApplicationComponentWorkspaceDraft *draft,
                                                     size_t index,
                                                     UmiApplicationComponentRegion region);
UmiStatus
umi_application_component_workspace_draft_set_weight(UmiApplicationComponentWorkspaceDraft *draft,
                                                     size_t index, uint32_t weight);
UmiStatus
umi_application_component_workspace_draft_set_title(UmiApplicationComponentWorkspaceDraft *draft,
                                                    const char *title);
UmiStatus umi_application_component_workspace_draft_project(
    const UmiApplicationComponentWorkspaceDraft *draft, UmiApplicationComponentLayout *out_layout);
void umi_application_component_workspace_draft_mark_saved(
    UmiApplicationComponentWorkspaceDraft *draft);

#ifdef __cplusplus
}
#endif

#endif
