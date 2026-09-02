/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_component/test_workspace_rebase.c
 *
 * PURPOSE:
 *   Verify a customised workspace can adopt a richer Framework recipe while
 *   retaining matching placement choices and reporting added components.
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
#include <string.h>

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
  const UmiApplicationComponentRecipe *learning =
      umi_application_component_recipe_catalogue_find("org.umicom.workspace.trader.learning");
  const UmiApplicationComponentRecipe *standard =
      umi_application_component_recipe_catalogue_find("org.umicom.workspace.trader.standard");
  UmiApplicationComponentWorkspaceDraft draft;
  UmiApplicationComponentRebaseReport report;
  size_t chart_index;

  assert(learning != NULL && standard != NULL);
  assert(umi_application_component_workspace_draft_init(&draft, learning) == UMI_STATUS_OK);
  assert(umi_application_component_workspace_draft_find(&draft, "umicom.trading.chart",
                                                        &chart_index) != NULL);
  draft.slots[chart_index].weight = 9U;
  assert(umi_application_component_workspace_rebase(&draft, standard, 0, &report) == UMI_STATUS_OK);
  assert(report.preserved_count == learning->slot_count);
  assert(report.added_count == standard->slot_count - learning->slot_count);
  assert(report.removed_count == 0U);
  assert(strcmp(draft.recipe_id, standard->recipe_id) == 0);
  assert(umi_application_component_workspace_draft_find(&draft, "umicom.trading.chart",
                                                        &chart_index) != NULL);
  assert(draft.slots[chart_index].weight == 9U);
  assert(draft.dirty);
  return 0;
}
