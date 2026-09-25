/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/open_tms/risk/liquidity.h
 *
 * PURPOSE:
 *   Evaluate Liquidity Risk through the Framework-owned Open TMS control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OPEN_TMS_RISK_LIQUIDITY_H
#define UMICOM_OPEN_TMS_RISK_LIQUIDITY_H

#include "umicom/open_tms/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_open_tms_evaluate_risk_liquidity(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
