/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/calibration_cache.h
 *
 * PURPOSE:
 *   Cache calibrated object fingerprints and fit error evidence.
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

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_CALIBRATION_CACHE_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_CALIBRATION_CACHE_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the enterprise calibration cache entry data shared with callers of this public
 * contract.
 */
typedef struct UmiEnterpriseCalibrationCacheEntry { char object_id[UMI_ENTERPRISE_ID_CAPACITY]; uint64_t input_fingerprint; double fit_error; } UmiEnterpriseCalibrationCacheEntry;
/**
 * Represent the enterprise calibration cache data shared with callers of this public
 * contract.
 */
typedef struct UmiEnterpriseCalibrationCache { UmiEnterpriseCalibrationCacheEntry entries[32U]; size_t count; } UmiEnterpriseCalibrationCache;
/* Initialise an empty calibration cache. */
void umi_enterprise_calibration_cache_init(UmiEnterpriseCalibrationCache *cache);
/* Insert or replace a calibrated object by canonical object identity. */
UmiStatus umi_enterprise_calibration_cache_put(UmiEnterpriseCalibrationCache *cache,const UmiEnterpriseCalibrationCacheEntry *entry);
/* Look up one calibrated object by identity and input fingerprint. */
const UmiEnterpriseCalibrationCacheEntry *umi_enterprise_calibration_cache_find(const UmiEnterpriseCalibrationCache *cache,const char *object_id,uint64_t input_fingerprint);

#ifdef __cplusplus
}
#endif

#endif
