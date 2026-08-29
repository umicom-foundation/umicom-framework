/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experience/profile.h
 *
 * PURPOSE:
 *   Combine command, form and UI-state policies into one experience profile
 *   that can be attached to a portable workspace recipe.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_EXPERIENCE_PROFILE_H
#define UMICOM_APPLICATION_EXPERIENCE_PROFILE_H

#include "umicom/application/component/recipe.h"
#include "umicom/application/experience/state_store.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationExperienceCommandPolicy {
  int confirm_destructive;
  int enable_undo;
  int show_shortcuts;
  uint32_t history_limit;
} UmiApplicationExperienceCommandPolicy;

typedef struct UmiApplicationExperienceFormPolicy {
  UmiApplicationExperienceValidationTiming validation_timing;
  int autosave;
  int show_inline_help;
  int preserve_draft;
} UmiApplicationExperienceFormPolicy;

typedef struct UmiApplicationExperienceStatePolicy {
  UmiApplicationExperienceDensity density;
  int show_empty_guidance;
  int announce_changes;
  uint32_t notification_limit;
} UmiApplicationExperienceStatePolicy;

typedef struct UmiApplicationExperienceProfile {
  uint32_t struct_size;
  uint32_t api_version;
  const char *profile_id;
  const char *recipe_id;
  const char *application_id;
  UmiApplicationComponentRecipeAudience audience;
  const char *primary_command_id;
  const char *primary_form_id;
  UmiApplicationExperienceCommandPolicy command_policy;
  UmiApplicationExperienceFormPolicy form_policy;
  UmiApplicationExperienceStatePolicy state_policy;
} UmiApplicationExperienceProfile;

UmiStatus
umi_application_experience_profile_validate(const UmiApplicationExperienceProfile *profile);
int umi_application_experience_profile_matches_recipe(
    const UmiApplicationExperienceProfile *profile, const UmiApplicationComponentRecipe *recipe);

#ifdef __cplusplus
}
#endif

#endif
