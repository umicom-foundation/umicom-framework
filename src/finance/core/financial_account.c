/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/financial_account.c
 *
 * PURPOSE:
 *   Implement richer account metadata without replacing existing UmiFinancialAccount.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/financial_account.h"

#include <string.h>
/* Initialize the typed financial record. */
UmiStatus umi_financial_account_init(UmiFinancialCoreAccount *item,const char *id,const char *name,const char *parent_id,const char *code){UmiStatus st; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT; memset(item,0,sizeof *item); st=umi_financial_id_assign(&item->account_id,id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; st=umi_financial_core_copy(item->name,sizeof item->name,name); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; st=umi_financial_id_assign(&item->parent_id,parent_id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; st=umi_financial_core_copy(item->code,sizeof item->code,code); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; item->active=true; return UMI_STATUS_OK;}
/* Validate the typed financial record. */
bool umi_financial_account_is_valid(const UmiFinancialCoreAccount *item){return item!=NULL&&umi_financial_id_is_valid(&item->account_id)&&item->name[0]!='\0'&&umi_financial_id_is_valid(&item->parent_id)&&item->code[0]!='\0';}
