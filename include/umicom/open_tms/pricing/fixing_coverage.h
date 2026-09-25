/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/open_tms/pricing/fixing_coverage.h
 *
 * PURPOSE:
 *   Evaluate Fixing Coverage through the Framework-owned Open TMS control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OPEN_TMS_PRICING_FIXING_COVERAGE_H
#define UMICOM_OPEN_TMS_PRICING_FIXING_COVERAGE_H

#include "umicom/open_tms/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_open_tms_evaluate_pricing_fixing_coverage(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
