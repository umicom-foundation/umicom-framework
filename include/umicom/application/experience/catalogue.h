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
#ifndef UMICOM_APPLICATION_EXPERIENCE_CATALOGUE_H
#define UMICOM_APPLICATION_EXPERIENCE_CATALOGUE_H

#include "umicom/application/experience/profile.h"

#ifdef __cplusplus
extern "C" {
#endif

size_t umi_application_experience_profile_catalogue_count(void);
const UmiApplicationExperienceProfile *
umi_application_experience_profile_catalogue_at(size_t index);
const UmiApplicationExperienceProfile *
umi_application_experience_profile_catalogue_find(const char *profile_id);
const UmiApplicationExperienceProfile *
umi_application_experience_profile_catalogue_for_recipe(const char *recipe_id);
const UmiApplicationExperienceProfile *umi_application_experience_profile_catalogue_recommend(
    const char *application_id, UmiApplicationComponentRecipeAudience audience);
size_t umi_application_experience_profile_catalogue_application_count(const char *application_id);
UmiStatus umi_application_experience_profile_catalogue_validate(void);

#ifdef __cplusplus
}
#endif

#endif
