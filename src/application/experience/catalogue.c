/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience/catalogue.c
 *
 * PURPOSE:
 *   Aggregate, query and cross-check application experience profiles against
 *   the authoritative portfolio and workspace recipe catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/experience/catalogue.h"

#include <string.h>

#include "umicom/application/component/recipe_catalogue.h"
#include "umicom/application/portfolio.h"

#include "profiles/profiles.inc"

static const UmiApplicationExperienceProfile *const PROFILES[] = {
#include "profiles/profile_records.inc"
};

size_t umi_application_experience_profile_catalogue_count(void) {
  return sizeof(PROFILES) / sizeof(PROFILES[0]);
}

const UmiApplicationExperienceProfile *
umi_application_experience_profile_catalogue_at(size_t index) {
  return index < umi_application_experience_profile_catalogue_count() ? PROFILES[index] : NULL;
}

const UmiApplicationExperienceProfile *
umi_application_experience_profile_catalogue_find(const char *profile_id) {
  size_t index;
  if (profile_id == NULL)
    return NULL;
  for (index = 0U; index < umi_application_experience_profile_catalogue_count(); ++index) {
    if (strcmp(PROFILES[index]->profile_id, profile_id) == 0)
      return PROFILES[index];
  }
  return NULL;
}

const UmiApplicationExperienceProfile *
umi_application_experience_profile_catalogue_for_recipe(const char *recipe_id) {
  size_t index;
  if (recipe_id == NULL)
    return NULL;
  for (index = 0U; index < umi_application_experience_profile_catalogue_count(); ++index) {
    if (strcmp(PROFILES[index]->recipe_id, recipe_id) == 0)
      return PROFILES[index];
  }
  return NULL;
}

const UmiApplicationExperienceProfile *umi_application_experience_profile_catalogue_recommend(
    const char *application_id, UmiApplicationComponentRecipeAudience audience) {
  size_t index;
  const UmiApplicationExperienceProfile *standard = NULL;
  if (application_id == NULL)
    return NULL;
  for (index = 0U; index < umi_application_experience_profile_catalogue_count(); ++index) {
    if (strcmp(PROFILES[index]->application_id, application_id) != 0)
      continue;
    if (PROFILES[index]->audience == audience)
      return PROFILES[index];
    if (PROFILES[index]->audience == UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_STANDARD)
      standard = PROFILES[index];
  }
  return standard;
}

size_t umi_application_experience_profile_catalogue_application_count(const char *application_id) {
  size_t index;
  size_t count = 0U;
  if (application_id == NULL)
    return 0U;
  for (index = 0U; index < umi_application_experience_profile_catalogue_count(); ++index) {
    if (strcmp(PROFILES[index]->application_id, application_id) == 0)
      count += 1U;
  }
  return count;
}

UmiStatus umi_application_experience_profile_catalogue_validate(void) {
  size_t index;
  if (umi_application_experience_profile_catalogue_count() !=
      umi_application_component_recipe_catalogue_count())
    return UMI_STATUS_INVALID_STATE;
  for (index = 0U; index < umi_application_experience_profile_catalogue_count(); ++index) {
    const UmiApplicationExperienceProfile *profile = PROFILES[index];
    const UmiApplicationComponentRecipe *recipe;
    size_t previous;
    UmiStatus status = umi_application_experience_profile_validate(profile);
    if (status != UMI_STATUS_OK)
      return status;
    if (umi_application_portfolio_find(profile->application_id) == NULL)
      return UMI_STATUS_NOT_FOUND;
    recipe = umi_application_component_recipe_catalogue_find(profile->recipe_id);
    if (!umi_application_experience_profile_matches_recipe(profile, recipe))
      return UMI_STATUS_INVALID_STATE;
    for (previous = 0U; previous < index; ++previous) {
      if (strcmp(PROFILES[previous]->profile_id, profile->profile_id) == 0 ||
          strcmp(PROFILES[previous]->recipe_id, profile->recipe_id) == 0)
        return UMI_STATUS_ALREADY_EXISTS;
    }
  }
  return UMI_STATUS_OK;
}
