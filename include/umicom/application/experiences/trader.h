/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experiences/trader.h
 *
 * PURPOSE:
 *   Publish the canonical Framework-owned product experience for Umicom Trader.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_EXPERIENCES_TRADER_H
#define UMICOM_APPLICATION_EXPERIENCES_TRADER_H

#include "umicom/application/experience.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the application experience trader operation used by this module and its client
 * applications.
 */
const UmiApplicationExperienceDefinition *umi_application_experience_trader(void);

#ifdef __cplusplus
}
#endif

#endif
