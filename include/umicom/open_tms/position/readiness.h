/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/open_tms/position/readiness.h
 *
 * PURPOSE:
 *   Evaluate Position Readiness through the Framework-owned Open TMS control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OPEN_TMS_POSITION_READINESS_H
#define UMICOM_OPEN_TMS_POSITION_READINESS_H

#include "umicom/open_tms/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_open_tms_evaluate_position_readiness(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
