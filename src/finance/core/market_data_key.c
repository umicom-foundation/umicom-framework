/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/market_data_key.c
 *
 * PURPOSE:
 *   Implement canonical market-data keys.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/market_data_key.h"

#include <string.h>
/* Initialize the typed financial record. */
UmiStatus umi_market_data_key_init(UmiMarketDataKey *item,const char *id,const char *name,const char *code,uint32_t state){UmiStatus st; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT; memset(item,0,sizeof *item); st=umi_financial_id_assign(&item->key_id,id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; st=umi_financial_core_copy(item->name,sizeof item->name,name); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; st=umi_financial_core_copy(item->code,sizeof item->code,code); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; item->state=state; item->active=true; return UMI_STATUS_OK;}
/* Validate the typed financial record. */
bool umi_market_data_key_is_valid(const UmiMarketDataKey *item){return item!=NULL&&umi_financial_id_is_valid(&item->key_id)&&item->name[0]!='\0'&&item->code[0]!='\0';}
