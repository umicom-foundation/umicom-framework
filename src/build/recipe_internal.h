/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/recipe_internal.h
 *
 * PURPOSE:
 *   Share private owned recipe storage and schema admission helpers across the native recipe
 * service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BUILD_RECIPE_INTERNAL_H
#define UMICOM_BUILD_RECIPE_INTERNAL_H
#include "umicom/build/recipe.h"
#include "umicom/language_runtime/json_tree.h"
struct UmiBuildRecipe {
  UmiBuildRecipeInfo info;
  UmiBuildRecipeInput inputs[UMI_BUILD_RECIPE_MAX_INPUTS];
  UmiBuildRecipeStage stages[UMI_BUILD_RECIPE_MAX_STAGES];
};
/* These helpers are recipe-specific admission policy; JSON grammar, hashing,
 * relative-path structure and task graph traversal remain shared services. */
UmiStatus UmiRecipeDecode(const UmiJsonTree *tree, UmiBuildRecipe *recipe,
                          UmiBuildRecipeReport *report);
UmiStatus UmiRecipeValidate(const UmiBuildRecipe *recipe, UmiBuildRecipeReport *report);
UmiStatus UmiRecipeReport(UmiBuildRecipeReport *report, UmiStatus status, size_t stage,
                          const char *message);
#endif
