/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/financial_snapshot.c
 *
 * PURPOSE:
 *   Implement lightweight financial inventory snapshots.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/financial_snapshot.h"

#include <string.h>
/* Initialize the typed financial record. */
UmiStatus umi_financial_snapshot_init(UmiFinancialSnapshot *item,const char *id,const char *name,const char *code,uint32_t state){UmiStatus st; if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT; memset(item,0,sizeof *item); st=umi_financial_id_assign(&item->snapshot_id,id); if(st!=UMI_STATUS_OK)return st; st=umi_financial_core_copy(item->name,sizeof item->name,name); if(st!=UMI_STATUS_OK)return st; st=umi_financial_core_copy(item->code,sizeof item->code,code); if(st!=UMI_STATUS_OK)return st; item->state=state; item->active=true; return UMI_STATUS_OK;}
/* Validate the typed financial record. */
bool umi_financial_snapshot_is_valid(const UmiFinancialSnapshot *item){return item!=NULL&&umi_financial_id_is_valid(&item->snapshot_id)&&item->name[0]!='\0'&&item->code[0]!='\0';}
