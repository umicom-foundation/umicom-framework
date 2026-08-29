/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience/profile.c
 *
 * PURPOSE:
 *   Validate complete experience policies and prove that each profile belongs
 *   to the application, audience and workspace recipe it declares.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/experience/profile.h"

#include <string.h>

static int boolean_valid(int value) { return value == 0 || value == 1; }

UmiStatus
umi_application_experience_profile_validate(const UmiApplicationExperienceProfile *profile) {
  if (profile == NULL || !umi_application_experience_identifier_valid(profile->profile_id) ||
      !umi_application_experience_identifier_valid(profile->recipe_id) ||
      !umi_application_experience_identifier_valid(profile->application_id) ||
      !umi_application_experience_identifier_valid(profile->primary_command_id) ||
      !umi_application_experience_identifier_valid(profile->primary_form_id))
    return UMI_STATUS_INVALID_ARGUMENT;
  if (profile->struct_size < sizeof(*profile) ||
      profile->api_version != UMI_APPLICATION_EXPERIENCE_API_VERSION ||
      profile->audience < UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_LEARNING ||
      profile->audience > UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_FOCUS ||
      profile->command_policy.history_limit == 0U ||
      profile->command_policy.history_limit > UMI_APPLICATION_EXPERIENCE_HISTORY_CAPACITY ||
      profile->form_policy.validation_timing < UMI_APPLICATION_EXPERIENCE_VALIDATE_MANUALLY ||
      profile->form_policy.validation_timing > UMI_APPLICATION_EXPERIENCE_VALIDATE_ON_SUBMIT ||
      profile->state_policy.density < UMI_APPLICATION_EXPERIENCE_DENSITY_COMFORTABLE ||
      profile->state_policy.density > UMI_APPLICATION_EXPERIENCE_DENSITY_COMPACT ||
      profile->state_policy.notification_limit == 0U ||
      !boolean_valid(profile->command_policy.confirm_destructive) ||
      !boolean_valid(profile->command_policy.enable_undo) ||
      !boolean_valid(profile->command_policy.show_shortcuts) ||
      !boolean_valid(profile->form_policy.autosave) ||
      !boolean_valid(profile->form_policy.show_inline_help) ||
      !boolean_valid(profile->form_policy.preserve_draft) ||
      !boolean_valid(profile->state_policy.show_empty_guidance) ||
      !boolean_valid(profile->state_policy.announce_changes))
    return UMI_STATUS_INVALID_STATE;
  return UMI_STATUS_OK;
}

int umi_application_experience_profile_matches_recipe(
    const UmiApplicationExperienceProfile *profile, const UmiApplicationComponentRecipe *recipe) {
  return profile != NULL && recipe != NULL &&
         umi_application_experience_profile_validate(profile) == UMI_STATUS_OK &&
         strcmp(profile->recipe_id, recipe->recipe_id) == 0 &&
         strcmp(profile->application_id, recipe->application_id) == 0 &&
         profile->audience == recipe->audience;
}
