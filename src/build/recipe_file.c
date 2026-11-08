/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/recipe_file.c
 *
 * PURPOSE:
 *   Compose bounded local file admission with immutable recipe parsing for all native clients.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "recipe_internal.h"
#include "umicom/platform/input_file.h"
UmiStatus UmiBuildRecipeReadFile(const char *absolute_path, UmiBuildRecipe **out_recipe,
                                 UmiBuildRecipeReport *report) {
  if (out_recipe == NULL)
    return UmiRecipeReport(report, UMI_STATUS_INVALID_ARGUMENT, SIZE_MAX,
                           "An output owner is required.");
  unsigned char *bytes = NULL;
  size_t size = 0U;
  UmiStatus status = UmiInputFileRead(absolute_path, UMI_BUILD_RECIPE_MAX_BYTES, &bytes, &size);
  if (status == UMI_STATUS_OK)
    status = UmiBuildRecipeOpen(bytes, size, out_recipe, report);
  else
    UmiRecipeReport(report, status, SIZE_MAX, "Recipe file could not be admitted or read.");
  UmiInputFileFree(bytes);
  return status;
}
