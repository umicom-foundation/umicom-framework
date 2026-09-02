/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experiences/tms.h
 *
 * PURPOSE:
 *   Publish the canonical Framework-owned product experience for Umicom TMS.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_EXPERIENCES_TMS_H
#define UMICOM_APPLICATION_EXPERIENCES_TMS_H

#include "umicom/application/experience.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the application experience tms operation used by this module and its client
 * applications.
 */
const UmiApplicationExperienceDefinition *umi_application_experience_tms(void);

#ifdef __cplusplus
}
#endif

#endif
