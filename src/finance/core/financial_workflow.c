/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/financial_workflow.c
 *
 * PURPOSE:
 *   Implement reusable financial workflow state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/financial_workflow.h"

#include <string.h>
/* Initialize the typed financial record. */
UmiStatus umi_financial_workflow_init(UmiFinancialWorkflow *item,const char *id,const char *name,const char *parent_id,uint32_t state){UmiStatus st; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT; memset(item,0,sizeof *item); st=umi_financial_id_assign(&item->workflow_id,id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; st=umi_financial_core_copy(item->name,sizeof item->name,name); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; st=umi_financial_id_assign(&item->parent_id,parent_id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st; item->state=state; item->active=true; return UMI_STATUS_OK;}
/* Validate the typed financial record. */
bool umi_financial_workflow_is_valid(const UmiFinancialWorkflow *item){return item!=NULL&&umi_financial_id_is_valid(&item->workflow_id)&&item->name[0]!='\0'&&umi_financial_id_is_valid(&item->parent_id);}
