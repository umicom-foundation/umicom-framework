/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/open_tms/liquidity/concentration.h
 *
 * PURPOSE:
 *   Evaluate Funding Concentration through the Framework-owned Open TMS control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OPEN_TMS_LIQUIDITY_CONCENTRATION_H
#define UMICOM_OPEN_TMS_LIQUIDITY_CONCENTRATION_H

#include "umicom/open_tms/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_open_tms_evaluate_liquidity_concentration(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
