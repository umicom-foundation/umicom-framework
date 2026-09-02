/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experiences/desktop.h
 *
 * PURPOSE:
 *   Publish the canonical Framework-owned product experience for Umicom Desk.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_EXPERIENCES_DESKTOP_H
#define UMICOM_APPLICATION_EXPERIENCES_DESKTOP_H

#include "umicom/application/experience.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the application experience desktop operation used by this module and its client
 * applications.
 */
const UmiApplicationExperienceDefinition *umi_application_experience_desktop(void);

#ifdef __cplusplus
}
#endif

#endif
