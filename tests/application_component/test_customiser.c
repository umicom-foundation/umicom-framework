/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_component/test_customiser.c
 *
 * PURPOSE:
 *   Verify user-facing workspace edits, lock protection, undo, redo, snapshots
 *   and saved-state tracking without requiring a graphical toolkit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/component/component.h"

#include <assert.h>

int main(void) {
  const UmiApplicationComponentRecipe *recipe =
      umi_application_component_recipe_catalogue_find("org.umicom.workspace.studio.learning");
  UmiApplicationComponentCustomiser customiser;
  UmiApplicationComponentCustomiserSnapshot snapshot;

  assert(recipe != NULL);
  assert(umi_application_component_customiser_init(&customiser, recipe) == UMI_STATUS_OK);
  assert(umi_application_component_customiser_remove(&customiser, "umicom.development.editor") ==
         UMI_STATUS_UNAVAILABLE);
  assert(umi_application_component_customiser_add(
             &customiser, "umicom.development.terminal", "terminal.secondary",
             UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 2U) == UMI_STATUS_OK);
  assert(umi_application_component_customiser_move(&customiser, "terminal.secondary", 1U) ==
         UMI_STATUS_OK);
  assert(umi_application_component_customiser_set_visible(&customiser, "terminal.secondary", 0) ==
         UMI_STATUS_OK);
  assert(umi_application_component_customiser_set_title(&customiser, "My Studio Workspace") ==
         UMI_STATUS_OK);
  assert(umi_application_component_customiser_snapshot(&customiser, &snapshot) == UMI_STATUS_OK);
  assert(snapshot.slot_count == 4U && snapshot.visible_count == 3U);
  assert(snapshot.undo_count == 4U && snapshot.dirty);
  assert(umi_application_component_customiser_undo(&customiser) == UMI_STATUS_OK);
  assert(umi_application_component_customiser_redo(&customiser) == UMI_STATUS_OK);
  umi_application_component_customiser_mark_saved(&customiser);
  assert(umi_application_component_customiser_snapshot(&customiser, &snapshot) == UMI_STATUS_OK);
  assert(!snapshot.dirty);
  return 0;
}
