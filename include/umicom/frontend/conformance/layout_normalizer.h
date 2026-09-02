/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/layout_normalizer.h
 *
 * PURPOSE:
 *   ratio, order and geometry normalisation before cross-frontend layout comparison.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_LAYOUT_NORMALIZER_H
#define UMICOM_FRONTEND_CONFORMANCE_LAYOUT_NORMALIZER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the fc layout normalizer ratios operation used by this module and its client
 * applications.
 */
void umi_fc_layout_normalizer_ratios(double *values,size_t count,double minimum,double maximum);
/**
 * Provide the fc layout normalizer orders operation used by this module and its client
 * applications.
 */
void umi_fc_layout_normalizer_orders(int32_t *orders,size_t count);

#ifdef __cplusplus
}
#endif
#endif
