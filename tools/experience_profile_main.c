/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/experience_profile_main.c
 *
 * PURPOSE:
 *   Let developers list, inspect and validate the interaction, form and
 *   UI-state profile paired with each Framework workspace recipe.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/experience/experience.h"

#include <stdio.h>
#include <string.h>

static int list_profiles(const char *application_id) {
  size_t index;
  for (index = 0U; index < umi_application_experience_profile_catalogue_count(); ++index) {
    const UmiApplicationExperienceProfile *profile =
        umi_application_experience_profile_catalogue_at(index);
    if (application_id != NULL && strcmp(profile->application_id, application_id) != 0)
      continue;
    (void)printf("%s | %s | %s\n", profile->profile_id, profile->application_id,
                 umi_application_component_recipe_audience_text(profile->audience));
  }
  return 0;
}

static int show_profile(const char *profile_id) {
  const UmiApplicationExperienceProfile *profile =
      umi_application_experience_profile_catalogue_find(profile_id);
  if (profile == NULL) {
    (void)fprintf(stderr, "Experience profile not found: %s\n", profile_id);
    return 2;
  }
  (void)printf("Profile: %s\n", profile->profile_id);
  (void)printf("Application: %s\n", profile->application_id);
  (void)printf("Workspace recipe: %s\n", profile->recipe_id);
  (void)printf("Primary command: %s\n", profile->primary_command_id);
  (void)printf("Primary form: %s\n", profile->primary_form_id);
  (void)printf("Validation: %d\n", (int)profile->form_policy.validation_timing);
  (void)printf("Density: %s\n",
               profile->state_policy.density == UMI_APPLICATION_EXPERIENCE_DENSITY_COMPACT
                   ? "compact"
                   : "comfortable");
  return 0;
}

static void print_help(void) {
  (void)printf("Usage:\n");
  (void)printf("  umicom-experience-profiles list [application-id]\n");
  (void)printf("  umicom-experience-profiles show <profile-id>\n");
  (void)printf("  umicom-experience-profiles validate\n");
}

int main(int argc, char **argv) {
  if (argc == 1 || (argc >= 2 && strcmp(argv[1], "list") == 0))
    return list_profiles(argc >= 3 ? argv[2] : NULL);
  if (argc == 3 && strcmp(argv[1], "show") == 0)
    return show_profile(argv[2]);
  if (argc == 2 && strcmp(argv[1], "validate") == 0) {
    UmiStatus status = umi_application_experience_profile_catalogue_validate();
    if (status != UMI_STATUS_OK) {
      (void)fprintf(stderr, "Experience catalogue validation failed: %s\n",
                    umi_status_text(status));
      return 3;
    }
    (void)printf("Validated %zu application experience profiles.\n",
                 umi_application_experience_profile_catalogue_count());
    return 0;
  }
  print_help();
  return 1;
}
