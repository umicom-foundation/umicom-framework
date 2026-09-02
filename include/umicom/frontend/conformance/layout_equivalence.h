/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/layout_equivalence.h
 *
 * PURPOSE:
 *   structural and geometric layout-equivalence scoring with configurable tolerance.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_LAYOUT_EQUIVALENCE_H
#define UMICOM_FRONTEND_CONFORMANCE_LAYOUT_EQUIVALENCE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the fc layout equivalence values operation used by this module and its client
 * applications.
 */
double umi_fc_layout_equivalence_values(const double *expected,const double *actual,size_t count,double tolerance);
/**
 * Provide the fc layout equivalence passes operation used by this module and its client
 * applications.
 */
bool umi_fc_layout_equivalence_passes(double score,double minimum);

#ifdef __cplusplus
}
#endif
#endif
