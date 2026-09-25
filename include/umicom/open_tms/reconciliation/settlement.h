/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/open_tms/reconciliation/settlement.h
 *
 * PURPOSE:
 *   Evaluate Settlement Reconciliation through the Framework-owned Open TMS control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OPEN_TMS_RECONCILIATION_SETTLEMENT_H
#define UMICOM_OPEN_TMS_RECONCILIATION_SETTLEMENT_H

#include "umicom/open_tms/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_open_tms_evaluate_reconciliation_settlement(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
