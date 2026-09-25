/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/open_tms/cash/account_coverage.h
 *
 * PURPOSE:
 *   Evaluate Treasury Account Coverage through the Framework-owned Open TMS control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OPEN_TMS_CASH_ACCOUNT_COVERAGE_H
#define UMICOM_OPEN_TMS_CASH_ACCOUNT_COVERAGE_H

#include "umicom/open_tms/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_open_tms_evaluate_cash_account_coverage(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
