/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/open_tms/reconciliation/trade.h
 *
 * PURPOSE:
 *   Evaluate Trade Reconciliation through the Framework-owned Open TMS control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OPEN_TMS_RECONCILIATION_TRADE_H
#define UMICOM_OPEN_TMS_RECONCILIATION_TRADE_H

#include "umicom/open_tms/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_open_tms_evaluate_reconciliation_trade(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
