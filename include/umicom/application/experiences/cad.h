/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experiences/cad.h
 *
 * PURPOSE:
 *   Publish the canonical Framework-owned product experience for Umicom CAD.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_EXPERIENCES_CAD_H
#define UMICOM_APPLICATION_EXPERIENCES_CAD_H

#include "umicom/application/experience.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the application experience cad operation used by this module and its client
 * applications.
 */
const UmiApplicationExperienceDefinition *umi_application_experience_cad(void);

#ifdef __cplusplus
}
#endif

#endif
