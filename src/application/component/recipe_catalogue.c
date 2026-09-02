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

/*
 * Return the number of records represented by application component recipe catalogue
 * without changing their state.
 */
size_t umi_application_component_recipe_catalogue_count(void) {
  return sizeof(RECIPES) / sizeof(RECIPES[0]);
}

/*
 * Find application component recipe catalogue while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiApplicationComponentRecipe *umi_application_component_recipe_catalogue_at(size_t index) {
  return index < umi_application_component_recipe_catalogue_count() ? RECIPES[index] : NULL;
}

/*
 * Find application component recipe catalogue while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiApplicationComponentRecipe *
umi_application_component_recipe_catalogue_find(const char *recipe_id) {
  size_t index;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (recipe_id == NULL)
    return NULL;
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < umi_application_component_recipe_catalogue_count(); ++index) {
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strcmp(RECIPES[index]->recipe_id, recipe_id) == 0)
      return RECIPES[index];
  }
  return NULL;
}

/*
 * Return the number of records represented by application component recipe catalogue
 * application without changing their state.
 */
size_t umi_application_component_recipe_catalogue_application_count(const char *application_id) {
  size_t index;
  size_t count = 0U;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (application_id == NULL)
    return 0U;
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < umi_application_component_recipe_catalogue_count(); ++index) {
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strcmp(RECIPES[index]->application_id, application_id) == 0)
      count += 1U;
  }
  return count;
}

/*
 * Find application component recipe catalogue application while leaving the underlying
 * catalogue or model owned by this module.
 */
const UmiApplicationComponentRecipe *
umi_application_component_recipe_catalogue_application_at(const char *application_id,
                                                          size_t application_index) {
  size_t index;
  size_t match_index = 0U;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (application_id == NULL)
    return NULL;
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < umi_application_component_recipe_catalogue_count(); ++index) {
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strcmp(RECIPES[index]->application_id, application_id) != 0)
      continue;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (match_index == application_index)
      return RECIPES[index];
    match_index += 1U;
  }
  return NULL;
}

/*
 * Provide the application component recipe catalogue recommend operation used by this
 * module and its client applications.
 */
const UmiApplicationComponentRecipe *umi_application_component_recipe_catalogue_recommend(
    const char *application_id, UmiApplicationComponentRecipeAudience audience) {
  size_t index;
  const UmiApplicationComponentRecipe *standard = NULL;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (application_id == NULL)
    return NULL;
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < umi_application_component_recipe_catalogue_count(); ++index) {
    const UmiApplicationComponentRecipe *recipe = RECIPES[index];
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(recipe->application_id, application_id) != 0)
      continue;
    /* Apply this branch only when its contract condition is satisfied. */
    if (recipe->audience == audience)
      return recipe;
    /* Apply this branch only when its contract condition is satisfied. */
    if (recipe->audience == UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_STANDARD)
      standard = recipe;
  }
  return standard;
}

/*
 * Check that application component recipe catalogue satisfies its contract before another
 * service relies on it.
 */
UmiStatus
umi_application_component_recipe_catalogue_validate(const UmiApplicationComponentRegistry *registry,
                                                    UmiApplicationComponentFrontend frontend) {
  size_t index;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (registry == NULL || !umi_application_component_frontend_valid(frontend))
    return UMI_STATUS_INVALID_ARGUMENT;
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < umi_application_component_recipe_catalogue_count(); ++index) {
    size_t previous;
    UmiStatus status =
        umi_application_component_recipe_validate(registry, RECIPES[index], frontend);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK)
      return status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (previous = 0U; previous < index; ++previous) {
      /* Keep the operation inside its valid bounds before reading, writing or adding data. */
      if (strcmp(RECIPES[previous]->recipe_id, RECIPES[index]->recipe_id) == 0)
        return UMI_STATUS_ALREADY_EXISTS;
    }
  }
  return UMI_STATUS_OK;
}
