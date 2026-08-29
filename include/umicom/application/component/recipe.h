/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/component/recipe.h
 *
 * PURPOSE:
 *   Describe named, portable workspace recipes built from reusable Framework
 *   components. A recipe is the instruction card for a Lego-like application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_COMPONENT_RECIPE_H
#define UMICOM_APPLICATION_COMPONENT_RECIPE_H

#include "umicom/application/component/layout.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION 1U
#define UMI_APPLICATION_COMPONENT_APPLICATION_ID_CAPACITY 96U
#define UMI_APPLICATION_COMPONENT_DESCRIPTION_CAPACITY 512U

typedef enum UmiApplicationComponentRecipeAudience {
  UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_LEARNING = 1,
  UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_STANDARD = 2,
  UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_FOCUS = 3
} UmiApplicationComponentRecipeAudience;

typedef struct UmiApplicationComponentRecipeSlot {
  const char *component_id;
  const char *instance_id;
  UmiApplicationComponentRegion region;
  uint32_t weight;
  int visible;
  int locked;
} UmiApplicationComponentRecipeSlot;

typedef struct UmiApplicationComponentRecipe {
  uint32_t struct_size;
  uint32_t api_version;
  const char *recipe_id;
  const char *application_id;
  const char *title;
  const char *description;
  UmiApplicationComponentRecipeAudience audience;
  const UmiApplicationComponentRecipeSlot *slots;
  size_t slot_count;
  const char *experience_profile_id;
} UmiApplicationComponentRecipe;

const char *
umi_application_component_recipe_audience_text(UmiApplicationComponentRecipeAudience audience);
UmiStatus umi_application_component_recipe_validate(const UmiApplicationComponentRegistry *registry,
                                                    const UmiApplicationComponentRecipe *recipe,
                                                    UmiApplicationComponentFrontend frontend);
UmiStatus umi_application_component_recipe_project(const UmiApplicationComponentRecipe *recipe,
                                                   UmiApplicationComponentLayout *out_layout);

#ifdef __cplusplus
}
#endif

#endif
