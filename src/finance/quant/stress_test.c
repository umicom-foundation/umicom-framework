/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/stress_test.c
 *
 * PURPOSE:
 *   Represent baseline and stressed portfolio valuations.
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

#include "umicom/finance/quant/stress_test.h"

#include <math.h>
#include <string.h>

/* Validate financial inputs before making the record observable to callers. */
UmiStatus umi_quant_stress_test_init(UmiQuantStressTest *record, double baseline_pv, double stressed_pv, double limit_loss)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (!(limit_loss >= 0.0)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    record->baseline_pv = baseline_pv;
    record->stressed_pv = stressed_pv;
    record->limit_loss = limit_loss;
    return UMI_STATUS_OK;
}

/* Return positive valuation loss under stress. */
double umi_quant_stress_test_loss(const UmiQuantStressTest *record)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) return 0.0;
    return fmax(record->baseline_pv - record->stressed_pv, 0.0);
}
