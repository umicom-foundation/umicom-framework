/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/density_equivalence.h
 *
 * PURPOSE:
 *   density-token equivalence scoring between renderer implementations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiFcDensityEquivalence { double expected; double actual; double tolerance; } UmiFcDensityEquivalence;
double umi_fc_density_equivalence_evaluate(const UmiFcDensityEquivalence *item);

#ifdef __cplusplus
}
#endif
#endif
