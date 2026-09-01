/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/feature_vector.h
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
 * This public contract isolates feature vector behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_FEATURE_VECTOR_H
#define INCLUDE_UMICOM_TRADING_FEATURE_VECTOR_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiFeatureVector { UmiMarketFactor factors[UMI_TRADING_MAX_FACTORS]; size_t count; } UmiFeatureVector;
void umi_feature_vector_init(UmiFeatureVector *vector);
UmiStatus umi_feature_vector_add(UmiFeatureVector *vector, const UmiMarketFactor *factor);
#ifdef __cplusplus
}
#endif
#endif
