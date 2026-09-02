/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/market_data_bundle.c
 *
 * PURPOSE:
 *   Collect bounded named quantitative market inputs for one valuation.
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

#include "umicom/finance/quant/market_data_bundle.h"

#include <math.h>
#include <string.h>

/*
 * Initialise quant market data bundle from caller-provided values so later operations
 * receive a known state.
 */
void umi_quant_market_data_bundle_init(UmiQuantMarketDataBundle *bundle){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(bundle!=NULL)memset(bundle,0,sizeof *bundle);}
/*
 * Add quant market data bundle only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_quant_market_data_bundle_add(UmiQuantMarketDataBundle *bundle,const char *name,double value){size_t i;UmiStatus status;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(bundle==NULL||name==NULL||!umi_quant_number_valid(value))return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(bundle->count>=128U)return UMI_STATUS_CAPACITY_EXCEEDED;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<bundle->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(bundle->values[i].name,name)==0)return UMI_STATUS_ALREADY_EXISTS;status=umi_quant_copy_text(bundle->values[bundle->count].name,sizeof bundle->values[bundle->count].name,name);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(status!=UMI_STATUS_OK)return status;bundle->values[bundle->count].value=value;bundle->count++;return UMI_STATUS_OK;}
/*
 * Find quant market data bundle while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_quant_market_data_bundle_find(const UmiQuantMarketDataBundle *bundle,const char *name,double *out_value){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(bundle==NULL||name==NULL||out_value==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<bundle->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(bundle->values[i].name,name)==0){*out_value=bundle->values[i].value;return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
