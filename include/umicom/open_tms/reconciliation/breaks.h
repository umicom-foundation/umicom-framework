/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/open_tms/reconciliation/breaks.h
 *
 * PURPOSE:
 *   Evaluate Treasury Reconciliation Breaks through the Framework-owned Open TMS control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OPEN_TMS_RECONCILIATION_BREAKS_H
#define UMICOM_OPEN_TMS_RECONCILIATION_BREAKS_H

#include "umicom/open_tms/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_open_tms_evaluate_reconciliation_breaks(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
