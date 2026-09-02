/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/frontend_health.h
 *
 * PURPOSE:
 *   aggregate frontend conformance health state and blocker counts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the fc frontend health data shared with callers of this public contract.
 */
typedef struct UmiFcFrontendHealth { size_t passed; size_t degraded; size_t failed; size_t blockers; double score; } UmiFcFrontendHealth;
/**
 * Provide the fc frontend health outcome operation used by this module and its client
 * applications.
 */
UmiFcOutcome umi_fc_frontend_health_outcome(const UmiFcFrontendHealth *health);
/**
 * Provide the fc frontend health healthy operation used by this module and its client
 * applications.
 */
bool umi_fc_frontend_health_healthy(const UmiFcFrontendHealth *health,double minimum);

#ifdef __cplusplus
}
#endif
#endif
