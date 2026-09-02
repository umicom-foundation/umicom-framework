/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/calibration_cache.c
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

#include "umicom/finance/enterprise/calibration_cache.h"

#include <string.h>
/*
 * Initialise enterprise calibration cache from caller-provided values so later operations
 * receive a known state.
 */
void umi_enterprise_calibration_cache_init(UmiEnterpriseCalibrationCache *c){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(c!=NULL)memset(c,0,sizeof *c); }
/*
 * Provide the enterprise calibration cache put operation used by this module and its
 * client applications.
 */
UmiStatus umi_enterprise_calibration_cache_put(UmiEnterpriseCalibrationCache *c,const UmiEnterpriseCalibrationCacheEntry *e){ size_t i; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL||e==NULL||!umi_quant_number_valid(e->fit_error)||e->fit_error<0.0)return UMI_STATUS_INVALID_ARGUMENT; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<c->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(c->entries[i].object_id,e->object_id)==0){c->entries[i]=*e;return UMI_STATUS_OK;} /* Protect caller-owned memory by checking that required state is available before it is used. */ if(c->count>=32U)return UMI_STATUS_CAPACITY_EXCEEDED; c->entries[c->count++]=*e; return UMI_STATUS_OK; }
/*
 * Find enterprise calibration cache while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiEnterpriseCalibrationCacheEntry *umi_enterprise_calibration_cache_find(const UmiEnterpriseCalibrationCache *c,const char *id,uint64_t fp){ size_t i; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL||id==NULL)return NULL; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<c->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(c->entries[i].object_id,id)==0&&c->entries[i].input_fingerprint==fp)return &c->entries[i]; return NULL; }
