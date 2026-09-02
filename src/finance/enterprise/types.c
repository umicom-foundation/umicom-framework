/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/types.c
 *
 * PURPOSE:
 *   Define common enterprise valuation, market-data and portfolio-risk primitives.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/enterprise/types.h"

#include <math.h>
#include <string.h>

/*
 * Provide the enterprise hash text operation used by this module and its client
 * applications.
 */
uint64_t umi_enterprise_hash_text(const char *text)
{
    uint64_t hash = UINT64_C(1469598103934665603);
    const unsigned char *cursor;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return 0U;
    cursor = (const unsigned char *)text;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != 0U) {
        hash ^= (uint64_t)(*cursor);
        hash *= UINT64_C(1099511628211);
        ++cursor;
    }
    return hash;
}

/*
 * Initialise enterprise metric from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_enterprise_metric_init(UmiEnterpriseNamedMetric *metric, const char *name, double value)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (metric == NULL || !umi_quant_number_valid(value)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(metric, 0, sizeof *metric);
    status = umi_quant_copy_text(metric->name, sizeof metric->name, name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    metric->value = value;
    return UMI_STATUS_OK;
}
