/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_component/test_recipe_catalogue.c
 *
 * PURPOSE:
 *   Verify complete recipe discovery, audience recommendation and contract
 *   validation for the official Umicom application portfolio.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Required fixture setup and checks also execute in optimized builds. */
#include "../support/assertions.h"
#include "umicom/application/component/component.h"

#include <assert.h>

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
  UmiApplicationComponentRegistry registry;
  const UmiApplicationComponentRecipe *recipe;

  umi_application_component_registry_init(&registry);
  assert(umi_application_component_registry_seed_catalogue(&registry) == UMI_STATUS_OK);
  /* Healthcare adds one product and its three audience layouts. The previous
   * fixed expectation is retained to document the catalogue expansion. */
#if 0
  assert(umi_application_component_recipe_catalogue_count() == 78U);
#endif
  assert(umi_application_component_recipe_catalogue_count() == 81U);
  assert(umi_application_component_recipe_catalogue_validate(
             &registry, UMI_APPLICATION_COMPONENT_FRONTEND_HEADLESS) == UMI_STATUS_OK);
  assert(umi_application_component_recipe_catalogue_application_count("org.umicom.studio") == 3U);
  recipe = umi_application_component_recipe_catalogue_recommend(
      "org.umicom.studio", UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_LEARNING);
  assert(recipe != NULL);
  assert(recipe->audience == UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_LEARNING);
  assert(umi_application_component_recipe_catalogue_find("org.umicom.workspace.trader.focus") !=
         NULL);
  assert(umi_application_component_recipe_catalogue_find("missing") == NULL);
  return 0;
}
