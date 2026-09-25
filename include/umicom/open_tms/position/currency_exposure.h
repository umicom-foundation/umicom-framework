/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/open_tms/position/currency_exposure.h
 *
 * PURPOSE:
 *   Evaluate Currency Exposure through the Framework-owned Open TMS control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OPEN_TMS_POSITION_CURRENCY_EXPOSURE_H
#define UMICOM_OPEN_TMS_POSITION_CURRENCY_EXPOSURE_H

#include "umicom/open_tms/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_open_tms_evaluate_position_currency_exposure(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
