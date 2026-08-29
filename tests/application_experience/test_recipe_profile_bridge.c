/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_experience/test_recipe_profile_bridge.c
 *
 * PURPOSE:
 *   Verify every workspace recipe explicitly names the matching application
 *   experience profile and that both catalogues agree in both directions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/experience/experience.h"

#include <assert.h>
#include <string.h>

int main(void) {
  size_t index;
  for (index = 0U; index < umi_application_component_recipe_catalogue_count(); ++index) {
    const UmiApplicationComponentRecipe *recipe =
        umi_application_component_recipe_catalogue_at(index);
    const UmiApplicationExperienceProfile *profile;
    assert(recipe != NULL);
    profile = umi_application_experience_profile_catalogue_for_recipe(recipe->recipe_id);
    assert(profile != NULL);
    assert(strcmp(recipe->experience_profile_id, profile->profile_id) == 0);
    assert(umi_application_experience_profile_matches_recipe(profile, recipe));
  }
  return 0;
}
