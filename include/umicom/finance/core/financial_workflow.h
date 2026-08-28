/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/financial_workflow.h
 *
 * PURPOSE:
 *   Define reusable financial workflow state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_FINANCIAL_WORKFLOW_H
#define UMICOM_FINANCE_CORE_FINANCIAL_WORKFLOW_H

#include "umicom/finance/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiFinancialWorkflow { UmiFinancialId workflow_id; UmiFinancialId parent_id; char name[UMI_FINANCIAL_CORE_NAME_CAPACITY]; uint32_t state; bool active; } UmiFinancialWorkflow;
/* Initialize the typed financial record. */ UmiStatus umi_financial_workflow_init(UmiFinancialWorkflow *item,const char *id,const char *name,const char *parent_id,uint32_t state);
/* Validate the typed financial record. */ bool umi_financial_workflow_is_valid(const UmiFinancialWorkflow *item);
#ifdef __cplusplus
}
#endif

#endif
