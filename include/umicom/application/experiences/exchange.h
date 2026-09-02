/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experiences/exchange.h
 *
 * PURPOSE:
 *   Publish the canonical Framework-owned product experience for Umicom Commodity Exchange.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_EXPERIENCES_EXCHANGE_H
#define UMICOM_APPLICATION_EXPERIENCES_EXCHANGE_H

#include "umicom/application/experience.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the application experience exchange operation used by this module and its client
 * applications.
 */
const UmiApplicationExperienceDefinition *umi_application_experience_exchange(void);

#ifdef __cplusplus
}
#endif

#endif
