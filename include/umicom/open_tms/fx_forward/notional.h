/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/open_tms/fx_forward/notional.h
 *
 * PURPOSE:
 *   Evaluate FX Forward Notional through the Framework-owned Open TMS control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OPEN_TMS_FX_FORWARD_NOTIONAL_H
#define UMICOM_OPEN_TMS_FX_FORWARD_NOTIONAL_H

#include "umicom/open_tms/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_open_tms_evaluate_fx_forward_notional(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
