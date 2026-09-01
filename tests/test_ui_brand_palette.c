/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ui_brand_palette.c
 *
 * PURPOSE:
 *   Verify that every shared Umicom palette is complete and that light and
 *   dark surfaces select a readable wordmark foreground.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/brand_palette.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

/* Exercise the public palette contract exactly as an application adapter does. */
int main(void) {
  const UmiUiBrandPalette *light = umi_ui_brand_palette_get(UMI_UI_BRAND_SURFACE_LIGHT);
  const UmiUiBrandPalette *dark = umi_ui_brand_palette_get(UMI_UI_BRAND_SURFACE_DARK);
  const UmiUiBrandPalette *contrast = umi_ui_brand_palette_get(UMI_UI_BRAND_SURFACE_HIGH_CONTRAST);

  /* Every published variant must be complete before an adapter consumes it. */
  assert(umi_ui_brand_palette_validate(light) == UMI_STATUS_OK);
  assert(umi_ui_brand_palette_validate(dark) == UMI_STATUS_OK);
  assert(umi_ui_brand_palette_validate(contrast) == UMI_STATUS_OK);

  /* Opposite foreground values keep the Umicom text visible on each surface. */
  assert(strcmp(light->logo_foreground, "#203247") == 0);
  assert(strcmp(dark->logo_foreground, "#F4F7FB") == 0);
  assert(strcmp(light->logo_foreground, dark->logo_foreground) != 0);

  /* Unknown variants return no palette instead of silently choosing one. */
  assert(umi_ui_brand_palette_get((UmiUiBrandSurface)0) == NULL);
  return EXIT_SUCCESS;
}
