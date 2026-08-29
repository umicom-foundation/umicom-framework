/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_component/test_selection_layout.c
 *
 * PURPOSE:
 *   Verify declarative selection, maturity filtering, portable layout
 *   validation and transactional headless workspace materialisation.
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
  UmiApplicationComponentRegistry components;
  UmiApplicationComponentFactoryRegistry factories;
  UmiApplicationComponentSelection selection;
  UmiApplicationComponentSelectionPlan plan;
  UmiApplicationComponentLayout layout;
  UmiApplicationComponentWorkspace workspace;
  const UmiApplicationComponentBundle *bundle;

  umi_application_component_registry_init(&components);
  umi_application_component_factory_registry_init(&factories);
  assert(umi_application_component_registry_seed_catalogue(&components) ==
         UMI_STATUS_OK);
  assert(umi_application_component_factory_registry_seed_headless(&factories) ==
         UMI_STATUS_OK);

  umi_application_component_selection_init(
      &selection, UMI_APPLICATION_COMPONENT_FRONTEND_HEADLESS);
  assert(umi_application_component_selection_add(
             &selection, "umicom.development.editor", "editor.main", 1) ==
         UMI_STATUS_OK);
  assert(umi_application_component_selection_add(
             &selection, "umicom.education.course", "course.optional", 0) ==
         UMI_STATUS_OK);
  assert(umi_application_component_selection_resolve(
             &components, &selection, &plan) == UMI_STATUS_OK);
  assert(plan.count == 1U && plan.skipped_optional == 1U);
  selection.count = UMI_APPLICATION_COMPONENT_SELECTION_CAPACITY + 1U;
  assert(umi_application_component_selection_resolve(
             &components, &selection, &plan) == UMI_STATUS_INVALID_STATE);

  bundle = umi_application_component_bundle_find("umicom.bundle.studio");
  assert(bundle != NULL && bundle->component_count == 8U);
  assert(umi_application_component_bundle_layout(bundle, &layout) == UMI_STATUS_OK);
  assert(umi_application_component_layout_validate(
             &components, &layout,
             UMI_APPLICATION_COMPONENT_FRONTEND_HEADLESS) == UMI_STATUS_OK);
  assert(umi_application_component_layout_materialise(
             &components, &factories, &layout,
             UMI_APPLICATION_COMPONENT_FRONTEND_HEADLESS,
             &workspace) == UMI_STATUS_OK);
  assert(workspace.instance_count == 8U);
  assert(workspace.instances[0].lifecycle == UMI_APPLICATION_COMPONENT_ACTIVE);
  umi_application_component_workspace_destroy(&workspace);
  assert(workspace.instance_count == 0U);
  return 0;
}
