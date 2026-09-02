/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/feature_vector.c
 *
 * PURPOSE:
 *   Store bounded market-research factors.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of feature vector. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/feature_vector.h"

/*
 * Initialise feature vector from caller-provided values so later operations receive a
 * known state.
 */
void umi_feature_vector_init(UmiFeatureVector *vector)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (vector != NULL) {
        vector->count = 0U;
    }
}

/* Add feature vector only after its inputs and available capacity have been checked. */
UmiStatus umi_feature_vector_add(UmiFeatureVector *vector,
                                 const UmiMarketFactor *factor)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (vector == NULL || factor == NULL || factor->name[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (vector->count >= UMI_TRADING_MAX_FACTORS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    vector->factors[vector->count] = *factor;
    vector->count++;
    return UMI_STATUS_OK;
}
