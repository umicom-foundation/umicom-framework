/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_component/test_workspace_policy.c
 *
 * PURPOSE:
 *   Verify beginner and standard workspace guardrails produce complete,
 *   understandable diagnostics for invalid component arrangements.
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

#include "umicom/base/text.h"

int main(void) {
  UmiApplicationComponentRegistry registry;
  UmiApplicationComponentWorkspaceDraft draft;
  UmiApplicationComponentWorkspacePolicy policy;
  UmiApplicationComponentDiagnosticReport report;
  const UmiApplicationComponentRecipe *recipe =
      umi_application_component_recipe_catalogue_find("org.umicom.workspace.studio.learning");

  assert(recipe != NULL);
  umi_application_component_registry_init(&registry);
  assert(umi_application_component_registry_seed_catalogue(&registry) == UMI_STATUS_OK);
  assert(umi_application_component_workspace_draft_init(&draft, recipe) == UMI_STATUS_OK);
  umi_application_component_workspace_policy_learning(&policy);
  assert(umi_application_component_workspace_policy_evaluate(
             &registry, &draft, UMI_APPLICATION_COMPONENT_FRONTEND_HEADLESS, &policy, &report) ==
         UMI_STATUS_OK);
  assert(umi_application_component_diagnostic_report_passed(&report));

  assert(umi_text_copy(draft.slots[1].component_id, sizeof(draft.slots[1].component_id),
                       "umicom.missing.component") == UMI_STATUS_OK);
  draft.slots[0].visible = 0;
  draft.slots[1].visible = 0;
  draft.slots[2].visible = 0;
  assert(umi_application_component_workspace_policy_evaluate(
             &registry, &draft, UMI_APPLICATION_COMPONENT_FRONTEND_HEADLESS, &policy, &report) ==
         UMI_STATUS_OK);
  assert(!umi_application_component_diagnostic_report_passed(&report));
  assert(report.error_count >= 3U);
  assert(umi_application_component_diagnostic_report_at(&report, 0U) != NULL);
  return 0;
}
