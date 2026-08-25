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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/quant/market_data_bundle.h"

#include <math.h>
#include <string.h>

void umi_quant_market_data_bundle_init(UmiQuantMarketDataBundle *bundle){if(bundle!=NULL)memset(bundle,0,sizeof *bundle);}
UmiStatus umi_quant_market_data_bundle_add(UmiQuantMarketDataBundle *bundle,const char *name,double value){size_t i;UmiStatus status;if(bundle==NULL||name==NULL||!umi_quant_number_valid(value))return UMI_STATUS_INVALID_ARGUMENT;if(bundle->count>=128U)return UMI_STATUS_CAPACITY_EXCEEDED;for(i=0U;i<bundle->count;++i)if(strcmp(bundle->values[i].name,name)==0)return UMI_STATUS_ALREADY_EXISTS;status=umi_quant_copy_text(bundle->values[bundle->count].name,sizeof bundle->values[bundle->count].name,name);if(status!=UMI_STATUS_OK)return status;bundle->values[bundle->count].value=value;bundle->count++;return UMI_STATUS_OK;}
UmiStatus umi_quant_market_data_bundle_find(const UmiQuantMarketDataBundle *bundle,const char *name,double *out_value){size_t i;if(bundle==NULL||name==NULL||out_value==NULL)return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<bundle->count;++i)if(strcmp(bundle->values[i].name,name)==0){*out_value=bundle->values[i].value;return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
