/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experience_catalogue.h
 *
 * PURPOSE:
 *   Publish the canonical Framework-owned experience catalogue for Umicom applications.
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

#include "umicom/application/experience.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Return the number of canonical experiences owned by Framework. */
size_t umi_application_experience_catalogue_count(void);

/* Borrow an experience by position, or return NULL outside the catalogue. */
const UmiApplicationExperienceDefinition *
umi_application_experience_catalogue_at(size_t index);

/* Borrow an experience by canonical or supported historical application ID. */
const UmiApplicationExperienceDefinition *
umi_application_experience_catalogue_find(const char *application_id);

/* Validate every experience and reject duplicate canonical application IDs. */
UmiStatus umi_application_experience_catalogue_validate(void);

#ifdef __cplusplus
}
#endif

#endif
