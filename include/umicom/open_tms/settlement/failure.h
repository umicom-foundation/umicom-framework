/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/open_tms/settlement/failure.h
 *
 * PURPOSE:
 *   Evaluate Settlement Failure through the Framework-owned Open TMS control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OPEN_TMS_SETTLEMENT_FAILURE_H
#define UMICOM_OPEN_TMS_SETTLEMENT_FAILURE_H

#include "umicom/open_tms/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_open_tms_evaluate_settlement_failure(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
