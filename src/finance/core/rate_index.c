/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/rate_index.c
 *
 * PURPOSE:
 *   Implement reusable rate-index metadata.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/rate_index.h"

#include <string.h>
/* Initialize the typed financial record. */
UmiStatus umi_rate_index_init(UmiRateIndex *item,const char *id,const char *name,const char *code,uint32_t state){UmiStatus st; if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT; memset(item,0,sizeof *item); st=umi_financial_id_assign(&item->index_id,id); if(st!=UMI_STATUS_OK)return st; st=umi_financial_core_copy(item->name,sizeof item->name,name); if(st!=UMI_STATUS_OK)return st; st=umi_financial_core_copy(item->code,sizeof item->code,code); if(st!=UMI_STATUS_OK)return st; item->state=state; item->active=true; return UMI_STATUS_OK;}
/* Validate the typed financial record. */
bool umi_rate_index_is_valid(const UmiRateIndex *item){return item!=NULL&&umi_financial_id_is_valid(&item->index_id)&&item->name[0]!='\0'&&item->code[0]!='\0';}
