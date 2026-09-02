/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experiences/media.h
 *
 * PURPOSE:
 *   Publish the canonical Framework-owned product experience for Umicom Media Studio.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_EXPERIENCES_MEDIA_H
#define UMICOM_APPLICATION_EXPERIENCES_MEDIA_H

#include "umicom/application/experience.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the application experience media operation used by this module and its client
 * applications.
 */
const UmiApplicationExperienceDefinition *umi_application_experience_media(void);

#ifdef __cplusplus
}
#endif

#endif
