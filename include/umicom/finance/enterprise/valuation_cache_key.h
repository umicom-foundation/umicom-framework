/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/valuation_cache_key.h
 *
 * PURPOSE:
 *   Build deterministic valuation-cache fingerprints from product, market and model identity.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_VALUATION_CACHE_KEY_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_VALUATION_CACHE_KEY_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEnterpriseValuationCacheKey { UmiFinancialId product_id; uint64_t market_fingerprint; char model_id[UMI_ENTERPRISE_ID_CAPACITY]; uint64_t fingerprint; } UmiEnterpriseValuationCacheKey;
/* Initialise a deterministic cache key for one product/market/model tuple. */
UmiStatus umi_enterprise_valuation_cache_key_init(UmiEnterpriseValuationCacheKey *key,const char *product_id,uint64_t market_fingerprint,const char *model_id);

#ifdef __cplusplus
}
#endif

#endif
