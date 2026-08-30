/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/valuation_cache_key.c
 *
 * PURPOSE:
 *   Build deterministic valuation-cache fingerprints from product, market and model identity.
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

#include "umicom/finance/enterprise/valuation_cache_key.h"

#include <string.h>
UmiStatus umi_enterprise_valuation_cache_key_init(UmiEnterpriseValuationCacheKey *k,const char *pid,uint64_t mfp,const char *model){ UmiStatus s; uint64_t h; if(k==NULL||mfp==0U)return UMI_STATUS_INVALID_ARGUMENT; memset(k,0,sizeof *k); s=umi_quant_copy_text(k->product_id.value,sizeof k->product_id.value,pid); if(s!=UMI_STATUS_OK)return s; s=umi_quant_copy_text(k->model_id,sizeof k->model_id,model); if(s!=UMI_STATUS_OK)return s; k->market_fingerprint=mfp; h=umi_enterprise_hash_text(k->product_id.value); h^=mfp+UINT64_C(0x9e3779b97f4a7c15)+(h<<6U)+(h>>2U); h^=umi_enterprise_hash_text(k->model_id); k->fingerprint=h; return UMI_STATUS_OK; }
