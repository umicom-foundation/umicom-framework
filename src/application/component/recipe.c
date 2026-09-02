/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipe.c
 *
 * PURPOSE:
 *   Validate workspace recipe metadata and project a recipe into the existing
 *   portable component layout understood by frontend factories.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/component/recipe.h"

#include <string.h>

/*
 * Provide the application component recipe audience text operation used by this module and
 * its client applications.
 */
const char *
umi_application_component_recipe_audience_text(UmiApplicationComponentRecipeAudience audience) {
  /* Select the behaviour associated with the requested command or state value. */
  switch (audience) {
  case UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_LEARNING:
    return "learning";
  case UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_STANDARD:
    return "standard";
  case UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_FOCUS:
    return "focus";
  default:
    return "unknown";
  }
}

/* Provide the validate metadata operation used by this module and its client applications. */
static UmiStatus validate_metadata(const UmiApplicationComponentRecipe *recipe) {
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (recipe == NULL || recipe->recipe_id == NULL || recipe->application_id == NULL ||
      recipe->title == NULL || recipe->description == NULL || recipe->slots == NULL ||
      recipe->experience_profile_id == NULL || recipe->recipe_id[0] == '\0' ||
      recipe->application_id[0] == '\0' || recipe->title[0] == '\0' ||
      recipe->description[0] == '\0' || recipe->experience_profile_id[0] == '\0')
    return UMI_STATUS_INVALID_ARGUMENT;
  /* Apply this branch only when its contract condition is satisfied. */
  if (recipe->struct_size < sizeof(*recipe) ||
      recipe->api_version != UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION ||
      recipe->slot_count == 0U || recipe->slot_count > UMI_APPLICATION_COMPONENT_LAYOUT_CAPACITY ||
      recipe->audience < UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_LEARNING ||
      recipe->audience > UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_FOCUS)
    return UMI_STATUS_INVALID_STATE;
  return UMI_STATUS_OK;
}

/*
 * Provide the application component recipe project operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_component_recipe_project(const UmiApplicationComponentRecipe *recipe,
                                                   UmiApplicationComponentLayout *out_layout) {
  size_t index;
  UmiStatus status = validate_metadata(recipe);
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (status != UMI_STATUS_OK || out_layout == NULL)
    return status != UMI_STATUS_OK ? status : UMI_STATUS_INVALID_ARGUMENT;
  status = umi_application_component_layout_init(out_layout, recipe->recipe_id, recipe->title);
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; status == UMI_STATUS_OK && index < recipe->slot_count; ++index) {
    const UmiApplicationComponentRecipeSlot *source = &recipe->slots[index];
    status = umi_application_component_layout_add(
        out_layout, source->component_id, source->instance_id, source->region, source->weight);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
      out_layout->slots[index].visible = source->visible != 0;
  }
  return status;
}

/*
 * Check that application component recipe satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_application_component_recipe_validate(const UmiApplicationComponentRegistry *registry,
                                                    const UmiApplicationComponentRecipe *recipe,
                                                    UmiApplicationComponentFrontend frontend) {
  UmiApplicationComponentLayout layout;
  UmiStatus status = validate_metadata(recipe);
  size_t index;
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status != UMI_STATUS_OK)
    return status;
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < recipe->slot_count; ++index) {
    const UmiApplicationComponentRecipeSlot *slot = &recipe->slots[index];
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (slot->component_id == NULL || slot->instance_id == NULL || slot->component_id[0] == '\0' ||
        slot->instance_id[0] == '\0' || slot->weight == 0U ||
        slot->region < UMI_APPLICATION_COMPONENT_REGION_PRIMARY ||
        slot->region > UMI_APPLICATION_COMPONENT_REGION_FLOATING)
      return UMI_STATUS_INVALID_STATE;
  }
  status = umi_application_component_recipe_project(recipe, &layout);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status != UMI_STATUS_OK)
    return status;
  return umi_application_component_layout_validate(registry, &layout, frontend);
}
