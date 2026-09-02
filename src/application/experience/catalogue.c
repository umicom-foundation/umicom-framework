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

/*
 * Return the number of records represented by application experience profile catalogue
 * without changing their state.
 */
size_t umi_application_experience_profile_catalogue_count(void) {
  return sizeof(PROFILES) / sizeof(PROFILES[0]);
}

/*
 * Find application experience profile catalogue while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiApplicationExperienceProfile *
umi_application_experience_profile_catalogue_at(size_t index) {
  return index < umi_application_experience_profile_catalogue_count() ? PROFILES[index] : NULL;
}

/*
 * Find application experience profile catalogue while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiApplicationExperienceProfile *
umi_application_experience_profile_catalogue_find(const char *profile_id) {
  size_t index;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (profile_id == NULL)
    return NULL;
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < umi_application_experience_profile_catalogue_count(); ++index) {
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strcmp(PROFILES[index]->profile_id, profile_id) == 0)
      return PROFILES[index];
  }
  return NULL;
}

/*
 * Provide the application experience profile catalogue for recipe operation used by this
 * module and its client applications.
 */
const UmiApplicationExperienceProfile *
umi_application_experience_profile_catalogue_for_recipe(const char *recipe_id) {
  size_t index;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (recipe_id == NULL)
    return NULL;
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < umi_application_experience_profile_catalogue_count(); ++index) {
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strcmp(PROFILES[index]->recipe_id, recipe_id) == 0)
      return PROFILES[index];
  }
  return NULL;
}

/*
 * Provide the application experience profile catalogue recommend operation used by this
 * module and its client applications.
 */
const UmiApplicationExperienceProfile *umi_application_experience_profile_catalogue_recommend(
    const char *application_id, UmiApplicationComponentRecipeAudience audience) {
  size_t index;
  const UmiApplicationExperienceProfile *standard = NULL;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (application_id == NULL)
    return NULL;
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < umi_application_experience_profile_catalogue_count(); ++index) {
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strcmp(PROFILES[index]->application_id, application_id) != 0)
      continue;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (PROFILES[index]->audience == audience)
      return PROFILES[index];
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (PROFILES[index]->audience == UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_STANDARD)
      standard = PROFILES[index];
  }
  return standard;
}

/*
 * Return the number of records represented by application experience profile catalogue
 * application without changing their state.
 */
size_t umi_application_experience_profile_catalogue_application_count(const char *application_id) {
  size_t index;
  size_t count = 0U;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (application_id == NULL)
    return 0U;
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < umi_application_experience_profile_catalogue_count(); ++index) {
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strcmp(PROFILES[index]->application_id, application_id) == 0)
      count += 1U;
  }
  return count;
}

/*
 * Check that application experience profile catalogue satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_application_experience_profile_catalogue_validate(void) {
  size_t index;
  /* Apply this branch only when its contract condition is satisfied. */
  if (umi_application_experience_profile_catalogue_count() !=
      umi_application_component_recipe_catalogue_count())
    return UMI_STATUS_INVALID_STATE;
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < umi_application_experience_profile_catalogue_count(); ++index) {
    const UmiApplicationExperienceProfile *profile = PROFILES[index];
    const UmiApplicationComponentRecipe *recipe;
    size_t previous;
    UmiStatus status = umi_application_experience_profile_validate(profile);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK)
      return status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_application_portfolio_find(profile->application_id) == NULL)
      return UMI_STATUS_NOT_FOUND;
    recipe = umi_application_component_recipe_catalogue_find(profile->recipe_id);
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!umi_application_experience_profile_matches_recipe(profile, recipe))
      return UMI_STATUS_INVALID_STATE;
    /* Visit each bounded item once so every record receives the same rule. */
    for (previous = 0U; previous < index; ++previous) {
      /* Use the stable identifier comparison to choose the matching record or policy. */
      if (strcmp(PROFILES[previous]->profile_id, profile->profile_id) == 0 ||
          strcmp(PROFILES[previous]->recipe_id, profile->recipe_id) == 0)
        return UMI_STATUS_ALREADY_EXISTS;
    }
  }
  return UMI_STATUS_OK;
}
