/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experiences/accountant.h
 *
 * PURPOSE:
 *   Publish the canonical Framework-owned product experience for Umicom Accountant.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_EXPERIENCES_ACCOUNTANT_H
#define UMICOM_APPLICATION_EXPERIENCES_ACCOUNTANT_H

#include "umicom/application/experience.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the application experience accountant operation used by this module and its
 * client applications.
 */
const UmiApplicationExperienceDefinition *umi_application_experience_accountant(void);

#ifdef __cplusplus
}
#endif

#endif
