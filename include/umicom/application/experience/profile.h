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

/**
 * Represent the application experience command policy data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationExperienceCommandPolicy {
  int confirm_destructive;
  int enable_undo;
  int show_shortcuts;
  uint32_t history_limit;
} UmiApplicationExperienceCommandPolicy;

/**
 * Represent the application experience form policy data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationExperienceFormPolicy {
  UmiApplicationExperienceValidationTiming validation_timing;
  int autosave;
  int show_inline_help;
  int preserve_draft;
} UmiApplicationExperienceFormPolicy;

/**
 * Represent the application experience state policy data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationExperienceStatePolicy {
  UmiApplicationExperienceDensity density;
  int show_empty_guidance;
  int announce_changes;
  uint32_t notification_limit;
} UmiApplicationExperienceStatePolicy;

/**
 * Represent the application experience profile data shared with callers of this public
 * contract.
 */
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

/**
 * Check that application experience profile satisfies its contract before another service
 * relies on it.
 */
UmiStatus
umi_application_experience_profile_validate(const UmiApplicationExperienceProfile *profile);
/**
 * Provide the application experience profile matches recipe operation used by this module
 * and its client applications.
 */
int umi_application_experience_profile_matches_recipe(
    const UmiApplicationExperienceProfile *profile, const UmiApplicationComponentRecipe *recipe);

#ifdef __cplusplus
}
#endif

#endif
