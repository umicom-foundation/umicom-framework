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

const char *
umi_application_component_recipe_audience_text(UmiApplicationComponentRecipeAudience audience) {
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

static UmiStatus validate_metadata(const UmiApplicationComponentRecipe *recipe) {
  if (recipe == NULL || recipe->recipe_id == NULL || recipe->application_id == NULL ||
      recipe->title == NULL || recipe->description == NULL || recipe->slots == NULL ||
      recipe->experience_profile_id == NULL || recipe->recipe_id[0] == '\0' ||
      recipe->application_id[0] == '\0' || recipe->title[0] == '\0' ||
      recipe->description[0] == '\0' || recipe->experience_profile_id[0] == '\0')
    return UMI_STATUS_INVALID_ARGUMENT;
  if (recipe->struct_size < sizeof(*recipe) ||
      recipe->api_version != UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION ||
      recipe->slot_count == 0U || recipe->slot_count > UMI_APPLICATION_COMPONENT_LAYOUT_CAPACITY ||
      recipe->audience < UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_LEARNING ||
      recipe->audience > UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_FOCUS)
    return UMI_STATUS_INVALID_STATE;
  return UMI_STATUS_OK;
}

UmiStatus umi_application_component_recipe_project(const UmiApplicationComponentRecipe *recipe,
                                                   UmiApplicationComponentLayout *out_layout) {
  size_t index;
  UmiStatus status = validate_metadata(recipe);
  if (status != UMI_STATUS_OK || out_layout == NULL)
    return status != UMI_STATUS_OK ? status : UMI_STATUS_INVALID_ARGUMENT;
  status = umi_application_component_layout_init(out_layout, recipe->recipe_id, recipe->title);
  for (index = 0U; status == UMI_STATUS_OK && index < recipe->slot_count; ++index) {
    const UmiApplicationComponentRecipeSlot *source = &recipe->slots[index];
    status = umi_application_component_layout_add(
        out_layout, source->component_id, source->instance_id, source->region, source->weight);
    if (status == UMI_STATUS_OK)
      out_layout->slots[index].visible = source->visible != 0;
  }
  return status;
}

UmiStatus umi_application_component_recipe_validate(const UmiApplicationComponentRegistry *registry,
                                                    const UmiApplicationComponentRecipe *recipe,
                                                    UmiApplicationComponentFrontend frontend) {
  UmiApplicationComponentLayout layout;
  UmiStatus status = validate_metadata(recipe);
  size_t index;
  if (status != UMI_STATUS_OK)
    return status;
  for (index = 0U; index < recipe->slot_count; ++index) {
    const UmiApplicationComponentRecipeSlot *slot = &recipe->slots[index];
    if (slot->component_id == NULL || slot->instance_id == NULL || slot->component_id[0] == '\0' ||
        slot->instance_id[0] == '\0' || slot->weight == 0U ||
        slot->region < UMI_APPLICATION_COMPONENT_REGION_PRIMARY ||
        slot->region > UMI_APPLICATION_COMPONENT_REGION_FLOATING)
      return UMI_STATUS_INVALID_STATE;
  }
  status = umi_application_component_recipe_project(recipe, &layout);
  if (status != UMI_STATUS_OK)
    return status;
  return umi_application_component_layout_validate(registry, &layout, frontend);
}
