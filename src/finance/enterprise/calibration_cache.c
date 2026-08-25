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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/enterprise/calibration_cache.h"

#include <string.h>
void umi_enterprise_calibration_cache_init(UmiEnterpriseCalibrationCache *c){ if(c!=NULL)memset(c,0,sizeof *c); }
UmiStatus umi_enterprise_calibration_cache_put(UmiEnterpriseCalibrationCache *c,const UmiEnterpriseCalibrationCacheEntry *e){ size_t i; if(c==NULL||e==NULL||!umi_quant_number_valid(e->fit_error)||e->fit_error<0.0)return UMI_STATUS_INVALID_ARGUMENT; for(i=0U;i<c->count;++i)if(strcmp(c->entries[i].object_id,e->object_id)==0){c->entries[i]=*e;return UMI_STATUS_OK;} if(c->count>=32U)return UMI_STATUS_CAPACITY_EXCEEDED; c->entries[c->count++]=*e; return UMI_STATUS_OK; }
const UmiEnterpriseCalibrationCacheEntry *umi_enterprise_calibration_cache_find(const UmiEnterpriseCalibrationCache *c,const char *id,uint64_t fp){ size_t i; if(c==NULL||id==NULL)return NULL; for(i=0U;i<c->count;++i)if(strcmp(c->entries[i].object_id,id)==0&&c->entries[i].input_fingerprint==fp)return &c->entries[i]; return NULL; }
