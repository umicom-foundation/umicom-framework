/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/frontend_health.h
 *
 * PURPOSE:
 *   aggregate frontend conformance health state and blocker counts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_FRONTEND_HEALTH_H
#define UMICOM_FRONTEND_CONFORMANCE_FRONTEND_HEALTH_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiFcFrontendHealth { size_t passed; size_t degraded; size_t failed; size_t blockers; double score; } UmiFcFrontendHealth;
UmiFcOutcome umi_fc_frontend_health_outcome(const UmiFcFrontendHealth *health);
bool umi_fc_frontend_health_healthy(const UmiFcFrontendHealth *health,double minimum);

#ifdef __cplusplus
}
#endif
#endif
