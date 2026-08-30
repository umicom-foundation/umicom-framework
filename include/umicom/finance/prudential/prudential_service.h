/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/prudential_service.h
 *
 * PURPOSE:
 *   Aggregate Framework prudential service readiness and control state.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_PRUDENTIAL_SERVICE_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_PRUDENTIAL_SERVICE_H

#include "umicom/finance/prudential/types.h"
#include "umicom/finance/prudential/prudential_snapshot.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPrudentialService { int capital_ready; int liquidity_ready; int stress_ready; int governance_ready; size_t blocked_controls; } UmiPrudentialService;
/* Evaluate aggregate prudential service readiness without overriding failed domain controls. */
UmiStatus umi_pru_prudential_service_evaluate(UmiPrudentialService *service, int capital_ready, int liquidity_ready, int stress_ready, int governance_ready, size_t blocked_controls);
/* Return one only when every prudential service domain is ready and no controls are blocked. */
int umi_pru_prudential_service_ready(const UmiPrudentialService *service);

#ifdef __cplusplus
}
#endif

#endif
