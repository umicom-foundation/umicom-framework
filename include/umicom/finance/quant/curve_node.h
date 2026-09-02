/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/curve_node.h
 *
 * PURPOSE:
 *   Represent one tenor/value node used by quantitative curves.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_CURVE_NODE_H
#define INCLUDE_UMICOM_FINANCE_QUANT_CURVE_NODE_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the quant curve node data shared with callers of this public contract.
 */
typedef struct UmiQuantCurveNode {
    int32_t tenor_days;
    double value;
} UmiQuantCurveNode;

/* Initialise and validate the curve node contract. */
UmiStatus umi_quant_curve_node_init(UmiQuantCurveNode *record, int32_t tenor_days, double value);

/* Return the node quote for generic curve consumers. */
double umi_quant_curve_node_quoted_value(const UmiQuantCurveNode *record);

#ifdef __cplusplus
}
#endif

#endif
