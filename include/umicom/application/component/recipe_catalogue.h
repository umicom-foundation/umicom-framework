/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/component/recipe_catalogue.h
 *
 * PURPOSE:
 *   Discover Framework-owned workspace recipes for every Umicom application
 *   without asking applications to duplicate layout definitions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_COMPONENT_RECIPE_CATALOGUE_H
#define UMICOM_APPLICATION_COMPONENT_RECIPE_CATALOGUE_H

#include "umicom/application/component/recipe.h"

#ifdef __cplusplus
extern "C" {
#endif

size_t umi_application_component_recipe_catalogue_count(void);
const UmiApplicationComponentRecipe *umi_application_component_recipe_catalogue_at(size_t index);
const UmiApplicationComponentRecipe *
umi_application_component_recipe_catalogue_find(const char *recipe_id);
size_t umi_application_component_recipe_catalogue_application_count(const char *application_id);
const UmiApplicationComponentRecipe *
umi_application_component_recipe_catalogue_application_at(const char *application_id,
                                                          size_t application_index);
const UmiApplicationComponentRecipe *umi_application_component_recipe_catalogue_recommend(
    const char *application_id, UmiApplicationComponentRecipeAudience audience);
UmiStatus
umi_application_component_recipe_catalogue_validate(const UmiApplicationComponentRegistry *registry,
                                                    UmiApplicationComponentFrontend frontend);

#ifdef __cplusplus
}
#endif

#endif
