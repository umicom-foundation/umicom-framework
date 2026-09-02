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

/**
 * Represent the application component draft slot data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationComponentDraftSlot {
  char component_id[UMI_APPLICATION_COMPONENT_ID_CAPACITY];
  char instance_id[UMI_APPLICATION_COMPONENT_ID_CAPACITY];
  UmiApplicationComponentRegion region;
  uint32_t order;
  uint32_t weight;
  int visible;
  int locked;
} UmiApplicationComponentDraftSlot;

/**
 * Represent the application component workspace draft data shared with callers of this
 * public contract.
 */
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

/**
 * Initialise application component workspace draft from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus
umi_application_component_workspace_draft_init(UmiApplicationComponentWorkspaceDraft *draft,
                                               const UmiApplicationComponentRecipe *recipe);
/**
 * Find application component workspace draft while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiApplicationComponentDraftSlot *
umi_application_component_workspace_draft_find(const UmiApplicationComponentWorkspaceDraft *draft,
                                               const char *instance_id, size_t *out_index);
/**
 * Add application component workspace draft only after its inputs and available capacity
 * have been checked.
 */
UmiStatus
umi_application_component_workspace_draft_insert(UmiApplicationComponentWorkspaceDraft *draft,
                                                 size_t index,
                                                 const UmiApplicationComponentDraftSlot *slot);
/**
 * Remove application component workspace draft while keeping the remaining records in a
 * valid and discoverable state.
 */
UmiStatus
umi_application_component_workspace_draft_remove(UmiApplicationComponentWorkspaceDraft *draft,
                                                 size_t index,
                                                 UmiApplicationComponentDraftSlot *out_removed);
/**
 * Provide the application component workspace draft move operation used by this module and
 * its client applications.
 */
UmiStatus
umi_application_component_workspace_draft_move(UmiApplicationComponentWorkspaceDraft *draft,
                                               size_t from_index, size_t to_index);
/**
 * Provide the application component workspace draft set visible operation used by this
 * module and its client applications.
 */
UmiStatus
umi_application_component_workspace_draft_set_visible(UmiApplicationComponentWorkspaceDraft *draft,
                                                      size_t index, int visible);
/**
 * Provide the application component workspace draft set region operation used by this
 * module and its client applications.
 */
UmiStatus
umi_application_component_workspace_draft_set_region(UmiApplicationComponentWorkspaceDraft *draft,
                                                     size_t index,
                                                     UmiApplicationComponentRegion region);
/**
 * Provide the application component workspace draft set weight operation used by this
 * module and its client applications.
 */
UmiStatus
umi_application_component_workspace_draft_set_weight(UmiApplicationComponentWorkspaceDraft *draft,
                                                     size_t index, uint32_t weight);
/**
 * Provide the application component workspace draft set title operation used by this
 * module and its client applications.
 */
UmiStatus
umi_application_component_workspace_draft_set_title(UmiApplicationComponentWorkspaceDraft *draft,
                                                    const char *title);
/**
 * Provide the application component workspace draft project operation used by this module
 * and its client applications.
 */
UmiStatus umi_application_component_workspace_draft_project(
    const UmiApplicationComponentWorkspaceDraft *draft, UmiApplicationComponentLayout *out_layout);
/**
 * Provide the application component workspace draft mark saved operation used by this
 * module and its client applications.
 */
void umi_application_component_workspace_draft_mark_saved(
    UmiApplicationComponentWorkspaceDraft *draft);

#ifdef __cplusplus
}
#endif

#endif
