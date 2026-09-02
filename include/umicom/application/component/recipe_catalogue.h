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

/**
 * Return the number of records represented by application component recipe catalogue
 * without changing their state.
 */
size_t umi_application_component_recipe_catalogue_count(void);
/**
 * Find application component recipe catalogue while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiApplicationComponentRecipe *umi_application_component_recipe_catalogue_at(size_t index);
/**
 * Find application component recipe catalogue while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiApplicationComponentRecipe *
umi_application_component_recipe_catalogue_find(const char *recipe_id);
/**
 * Return the number of records represented by application component recipe catalogue
 * application without changing their state.
 */
size_t umi_application_component_recipe_catalogue_application_count(const char *application_id);
/**
 * Find application component recipe catalogue application while leaving the underlying
 * catalogue or model owned by this module.
 */
const UmiApplicationComponentRecipe *
umi_application_component_recipe_catalogue_application_at(const char *application_id,
                                                          size_t application_index);
/**
 * Provide the application component recipe catalogue recommend operation used by this
 * module and its client applications.
 */
const UmiApplicationComponentRecipe *umi_application_component_recipe_catalogue_recommend(
    const char *application_id, UmiApplicationComponentRecipeAudience audience);
/**
 * Check that application component recipe catalogue satisfies its contract before another
 * service relies on it.
 */
UmiStatus
umi_application_component_recipe_catalogue_validate(const UmiApplicationComponentRegistry *registry,
                                                    UmiApplicationComponentFrontend frontend);

#ifdef __cplusplus
}
#endif

#endif
