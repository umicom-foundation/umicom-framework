/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experiences/marketplace.h
 *
 * PURPOSE:
 *   Publish the canonical Framework-owned product experience for Umicom Marketplace.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_EXPERIENCES_MARKETPLACE_H
#define UMICOM_APPLICATION_EXPERIENCES_MARKETPLACE_H

#include "umicom/application/experience.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the application experience marketplace operation used by this module and its
 * client applications.
 */
const UmiApplicationExperienceDefinition *umi_application_experience_marketplace(void);

#ifdef __cplusplus
}
#endif

#endif
