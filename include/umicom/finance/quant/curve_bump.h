/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/curve_bump.h
 *
 * PURPOSE:
 *   Apply deterministic parallel and single-node curve shocks.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_CURVE_BUMP_H
#define INCLUDE_UMICOM_FINANCE_QUANT_CURVE_BUMP_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Apply a parallel additive rate bump to a bounded curve. */
UmiStatus umi_quant_curve_bump_parallel(UmiQuantCurvePoint *nodes, size_t count, double bump);
/* Apply an additive bump to one node by index. */
UmiStatus umi_quant_curve_bump_node(UmiQuantCurvePoint *nodes, size_t count, size_t index, double bump);

#ifdef __cplusplus
}
#endif

#endif
