/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/feature_vector.c
 *
 * PURPOSE:
 *   Store bounded market-research factors.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source implements the small deterministic core of feature vector. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/feature_vector.h"

void umi_feature_vector_init(UmiFeatureVector *vector)
{
    if (vector != NULL) {
        vector->count = 0U;
    }
}

UmiStatus umi_feature_vector_add(UmiFeatureVector *vector,
                                 const UmiMarketFactor *factor)
{
    if (vector == NULL || factor == NULL || factor->name[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (vector->count >= UMI_TRADING_MAX_FACTORS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    vector->factors[vector->count] = *factor;
    vector->count++;
    return UMI_STATUS_OK;
}
