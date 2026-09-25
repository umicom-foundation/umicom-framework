/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/open_tms/pricing/present_value.h
 *
 * PURPOSE:
 *   Evaluate Present Value through the Framework-owned Open TMS control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OPEN_TMS_PRICING_PRESENT_VALUE_H
#define UMICOM_OPEN_TMS_PRICING_PRESENT_VALUE_H

#include "umicom/open_tms/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_open_tms_evaluate_pricing_present_value(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
