/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_component/test_recipe_portfolio.c
 *
 * PURPOSE:
 *   Prove every application in the authoritative portfolio has Learning,
 *   Standard and Focus recipes that project to a visible primary workspace.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/component/component.h"
#include "umicom/application/portfolio.h"

#include <assert.h>

int main(void) {
  size_t application_index;
  assert(umi_application_portfolio_count() == 26U);
  for (application_index = 0U; application_index < umi_application_portfolio_count();
       ++application_index) {
    const UmiApplicationDefinition *application = umi_application_portfolio_at(application_index);
    size_t recipe_index;
    assert(application != NULL);
    assert(umi_application_component_recipe_catalogue_application_count(
               application->application_id) == 3U);
    for (recipe_index = 0U; recipe_index < 3U; ++recipe_index) {
      const UmiApplicationComponentRecipe *recipe =
          umi_application_component_recipe_catalogue_application_at(application->application_id,
                                                                    recipe_index);
      UmiApplicationComponentLayout layout;
      size_t slot_index;
      size_t visible_primary_count = 0U;
      assert(recipe != NULL);
      assert(umi_application_component_recipe_project(recipe, &layout) == UMI_STATUS_OK);
      for (slot_index = 0U; slot_index < layout.slot_count; ++slot_index) {
        visible_primary_count +=
            layout.slots[slot_index].visible &&
            layout.slots[slot_index].region == UMI_APPLICATION_COMPONENT_REGION_PRIMARY;
      }
      assert(visible_primary_count > 0U);
    }
  }
  return 0;
}
