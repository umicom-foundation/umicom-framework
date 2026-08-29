/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipe_catalogue.c
 *
 * PURPOSE:
 *   Implement discovery, application filtering, recommendation and complete
 *   validation for Framework-owned workspace recipes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/component/recipe_catalogue.h"

#include <string.h>

#include "recipes/recipes.inc"

static const UmiApplicationComponentRecipe *const RECIPES[] = {
#include "recipes/recipe_records.inc"
};

size_t umi_application_component_recipe_catalogue_count(void) {
  return sizeof(RECIPES) / sizeof(RECIPES[0]);
}

const UmiApplicationComponentRecipe *umi_application_component_recipe_catalogue_at(size_t index) {
  return index < umi_application_component_recipe_catalogue_count() ? RECIPES[index] : NULL;
}

const UmiApplicationComponentRecipe *
umi_application_component_recipe_catalogue_find(const char *recipe_id) {
  size_t index;
  if (recipe_id == NULL)
    return NULL;
  for (index = 0U; index < umi_application_component_recipe_catalogue_count(); ++index) {
    if (strcmp(RECIPES[index]->recipe_id, recipe_id) == 0)
      return RECIPES[index];
  }
  return NULL;
}

size_t umi_application_component_recipe_catalogue_application_count(const char *application_id) {
  size_t index;
  size_t count = 0U;
  if (application_id == NULL)
    return 0U;
  for (index = 0U; index < umi_application_component_recipe_catalogue_count(); ++index) {
    if (strcmp(RECIPES[index]->application_id, application_id) == 0)
      count += 1U;
  }
  return count;
}

const UmiApplicationComponentRecipe *
umi_application_component_recipe_catalogue_application_at(const char *application_id,
                                                          size_t application_index) {
  size_t index;
  size_t match_index = 0U;
  if (application_id == NULL)
    return NULL;
  for (index = 0U; index < umi_application_component_recipe_catalogue_count(); ++index) {
    if (strcmp(RECIPES[index]->application_id, application_id) != 0)
      continue;
    if (match_index == application_index)
      return RECIPES[index];
    match_index += 1U;
  }
  return NULL;
}

const UmiApplicationComponentRecipe *umi_application_component_recipe_catalogue_recommend(
    const char *application_id, UmiApplicationComponentRecipeAudience audience) {
  size_t index;
  const UmiApplicationComponentRecipe *standard = NULL;
  if (application_id == NULL)
    return NULL;
  for (index = 0U; index < umi_application_component_recipe_catalogue_count(); ++index) {
    const UmiApplicationComponentRecipe *recipe = RECIPES[index];
    if (strcmp(recipe->application_id, application_id) != 0)
      continue;
    if (recipe->audience == audience)
      return recipe;
    if (recipe->audience == UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_STANDARD)
      standard = recipe;
  }
  return standard;
}

UmiStatus
umi_application_component_recipe_catalogue_validate(const UmiApplicationComponentRegistry *registry,
                                                    UmiApplicationComponentFrontend frontend) {
  size_t index;
  if (registry == NULL || !umi_application_component_frontend_valid(frontend))
    return UMI_STATUS_INVALID_ARGUMENT;
  for (index = 0U; index < umi_application_component_recipe_catalogue_count(); ++index) {
    size_t previous;
    UmiStatus status =
        umi_application_component_recipe_validate(registry, RECIPES[index], frontend);
    if (status != UMI_STATUS_OK)
      return status;
    for (previous = 0U; previous < index; ++previous) {
      if (strcmp(RECIPES[previous]->recipe_id, RECIPES[index]->recipe_id) == 0)
        return UMI_STATUS_ALREADY_EXISTS;
    }
  }
  return UMI_STATUS_OK;
}
