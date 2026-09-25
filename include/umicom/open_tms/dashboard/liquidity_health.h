/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/open_tms/dashboard/liquidity_health.h
 *
 * PURPOSE:
 *   Evaluate Liquidity Dashboard Health through the Framework-owned Open TMS control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OPEN_TMS_DASHBOARD_LIQUIDITY_HEALTH_H
#define UMICOM_OPEN_TMS_DASHBOARD_LIQUIDITY_HEALTH_H

#include "umicom/open_tms/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_open_tms_evaluate_dashboard_liquidity_health(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
