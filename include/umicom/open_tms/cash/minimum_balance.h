/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/open_tms/cash/minimum_balance.h
 *
 * PURPOSE:
 *   Evaluate Minimum Cash Buffer through the Framework-owned Open TMS control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OPEN_TMS_CASH_MINIMUM_BALANCE_H
#define UMICOM_OPEN_TMS_CASH_MINIMUM_BALANCE_H

#include "umicom/open_tms/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_open_tms_evaluate_cash_minimum_balance(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
