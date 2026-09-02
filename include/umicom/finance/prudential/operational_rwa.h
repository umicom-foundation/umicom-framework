/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/operational_rwa.h
 *
 * PURPOSE:
 *   Calculate operational risk-weighted assets from business indicator and loss multiplier.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_OPERATIONAL_RWA_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_OPERATIONAL_RWA_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the prudential operational rwa data shared with callers of this public
 * contract.
 */
typedef struct UmiPrudentialOperationalRwa { double business_indicator_component; double loss_multiplier; double conversion_factor; double rwa; } UmiPrudentialOperationalRwa;
/* Calculate operational RWA from business-indicator component, loss multiplier and conversion factor. */
UmiStatus umi_pru_operational_rwa_calculate(UmiPrudentialOperationalRwa *result, double business_indicator_component, double loss_multiplier, double conversion_factor);

#ifdef __cplusplus
}
#endif

#endif
