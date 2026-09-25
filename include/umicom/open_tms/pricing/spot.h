/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/open_tms/pricing/spot.h
 *
 * PURPOSE:
 *   Evaluate Treasury Spot Price through the Framework-owned Open TMS control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OPEN_TMS_PRICING_SPOT_H
#define UMICOM_OPEN_TMS_PRICING_SPOT_H

#include "umicom/open_tms/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_open_tms_evaluate_pricing_spot(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
