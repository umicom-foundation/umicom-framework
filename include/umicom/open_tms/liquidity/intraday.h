/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/open_tms/liquidity/intraday.h
 *
 * PURPOSE:
 *   Evaluate Intraday Liquidity through the Framework-owned Open TMS control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OPEN_TMS_LIQUIDITY_INTRADAY_H
#define UMICOM_OPEN_TMS_LIQUIDITY_INTRADAY_H

#include "umicom/open_tms/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_open_tms_evaluate_liquidity_intraday(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
