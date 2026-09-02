/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/curve_bump.c
 *
 * PURPOSE:
 *   Apply deterministic parallel and single-node curve shocks.
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

#include "umicom/finance/quant/curve_bump.h"

#include <math.h>
#include <string.h>

/*
 * Provide the quant curve bump parallel operation used by this module and its client
 * applications.
 */
UmiStatus umi_quant_curve_bump_parallel(UmiQuantCurvePoint *nodes, size_t count, double bump)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (nodes == NULL || count > UMI_QUANT_MAX_CURVE_NODES || !umi_quant_number_valid(bump)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < count; ++i) nodes[i].value += bump;
    return UMI_STATUS_OK;
}

/*
 * Provide the quant curve bump node operation used by this module and its client
 * applications.
 */
UmiStatus umi_quant_curve_bump_node(UmiQuantCurvePoint *nodes, size_t count, size_t index, double bump)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (nodes == NULL || index >= count || count > UMI_QUANT_MAX_CURVE_NODES) return UMI_STATUS_INVALID_ARGUMENT;
    nodes[index].value += bump;
    return UMI_STATUS_OK;
}
