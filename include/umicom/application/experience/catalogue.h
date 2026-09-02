/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experience/catalogue.h
 *
 * PURPOSE:
 *   Discover the Framework-owned experience profile for every application and
 *   workspace recipe through one stable portfolio catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_EXPERIENCE_PROFILE_CATALOGUE_H
#define UMICOM_APPLICATION_EXPERIENCE_PROFILE_CATALOGUE_H

#include "umicom/application/experience/profile.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Return the number of records represented by application experience profile catalogue
 * without changing their state.
 */
size_t umi_application_experience_profile_catalogue_count(void);
/**
 * Find application experience profile catalogue while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiApplicationExperienceProfile *
umi_application_experience_profile_catalogue_at(size_t index);
/**
 * Find application experience profile catalogue while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiApplicationExperienceProfile *
umi_application_experience_profile_catalogue_find(const char *profile_id);
/**
 * Provide the application experience profile catalogue for recipe operation used by this
 * module and its client applications.
 */
const UmiApplicationExperienceProfile *
umi_application_experience_profile_catalogue_for_recipe(const char *recipe_id);
/**
 * Provide the application experience profile catalogue recommend operation used by this
 * module and its client applications.
 */
const UmiApplicationExperienceProfile *umi_application_experience_profile_catalogue_recommend(
    const char *application_id, UmiApplicationComponentRecipeAudience audience);
/**
 * Return the number of records represented by application experience profile catalogue
 * application without changing their state.
 */
size_t umi_application_experience_profile_catalogue_application_count(const char *application_id);
/**
 * Check that application experience profile catalogue satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_application_experience_profile_catalogue_validate(void);

#ifdef __cplusplus
}
#endif

#endif
