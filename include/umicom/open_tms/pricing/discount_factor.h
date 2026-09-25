/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/open_tms/pricing/discount_factor.h
 *
 * PURPOSE:
 *   Evaluate Discount Factor through the Framework-owned Open TMS control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OPEN_TMS_PRICING_DISCOUNT_FACTOR_H
#define UMICOM_OPEN_TMS_PRICING_DISCOUNT_FACTOR_H

#include "umicom/open_tms/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_open_tms_evaluate_pricing_discount_factor(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
