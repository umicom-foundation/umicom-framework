/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experiences/web_studio.h
 *
 * PURPOSE:
 *   Publish the canonical Framework-owned product experience for Umicom Web Studio.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_EXPERIENCES_WEB_STUDIO_H
#define UMICOM_APPLICATION_EXPERIENCES_WEB_STUDIO_H

#include "umicom/application/experience.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the application experience web studio operation used by this module and its
 * client applications.
 */
const UmiApplicationExperienceDefinition *umi_application_experience_web_studio(void);

#ifdef __cplusplus
}
#endif

#endif
