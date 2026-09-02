/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/density_equivalence.h
 *
 * PURPOSE:
 *   density-token equivalence scoring between renderer implementations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_DENSITY_EQUIVALENCE_H
#define UMICOM_FRONTEND_CONFORMANCE_DENSITY_EQUIVALENCE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fc density equivalence data shared with callers of this public contract.
 */
typedef struct UmiFcDensityEquivalence { double expected; double actual; double tolerance; } UmiFcDensityEquivalence;
/**
 * Provide the fc density equivalence evaluate operation used by this module and its client
 * applications.
 */
double umi_fc_density_equivalence_evaluate(const UmiFcDensityEquivalence *item);

#ifdef __cplusplus
}
#endif
#endif
