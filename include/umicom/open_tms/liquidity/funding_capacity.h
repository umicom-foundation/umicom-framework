/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/open_tms/liquidity/funding_capacity.h
 *
 * PURPOSE:
 *   Evaluate Funding Capacity through the Framework-owned Open TMS control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OPEN_TMS_LIQUIDITY_FUNDING_CAPACITY_H
#define UMICOM_OPEN_TMS_LIQUIDITY_FUNDING_CAPACITY_H

#include "umicom/open_tms/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_open_tms_evaluate_liquidity_funding_capacity(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
