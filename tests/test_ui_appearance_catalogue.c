/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ui_appearance_catalogue.c
 *
 * PURPOSE:
 *   Verify the shared appearance catalogue supplies complete immutable presets
 *   and one editable profile for every Umicom application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/appearance_catalogue.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

/* Exercise catalogue lookup and model population as a thin application does. */
int main(void) {
  UmiUiAppearanceModel *model = NULL;
  UmiUiAppearanceProfile profile;
  char reason[192U];

  assert(umi_ui_appearance_catalogue_count() == 6U);
  assert(umi_ui_appearance_catalogue_find("umicom-retro", &profile) == UMI_STATUS_OK);
  assert(strcmp(profile.label, "Retro") == 0);
  assert(profile.built_in);
  assert(strcmp(profile.icon_resource, "branding/umicom-icon-on-dark.svg") == 0);
  assert(umi_ui_appearance_profile_validate(&profile, reason, sizeof(reason)) == UMI_STATUS_OK);
  assert(umi_ui_appearance_catalogue_find("umicom-neo", &profile) == UMI_STATUS_OK);
  assert(strcmp(profile.accent, "#6FD3FF") == 0);
  assert(umi_ui_appearance_catalogue_find("umicom-light", &profile) == UMI_STATUS_OK);
  assert(strcmp(profile.logo_resource, "branding/umicom-logo.svg") == 0);
  assert(umi_ui_appearance_catalogue_find("umicom-custom", &profile) == UMI_STATUS_OK);
  assert(!profile.built_in);
  assert(!profile.locked);

  assert(umi_ui_appearance_model_create(&model) == UMI_STATUS_OK);
  assert(umi_ui_appearance_catalogue_populate(model) == UMI_STATUS_OK);
  assert(umi_ui_appearance_model_count(model) == 6U);
  assert(umi_ui_appearance_model_active(model, &profile) == UMI_STATUS_OK);
  assert(strcmp(profile.profile_id, "umicom-dark") == 0);
  umi_ui_appearance_model_destroy(model);
  return EXIT_SUCCESS;
}
