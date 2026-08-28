/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/settlement_instruction.h
 *
 * PURPOSE:
 *   Describe provider-neutral settlement instructions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_SETTLEMENT_INSTRUCTION_H
#define UMICOM_FINANCE_CORE_SETTLEMENT_INSTRUCTION_H

#include "umicom/finance/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSettlementInstruction { UmiFinancialId instruction_id; UmiFinancialId parent_id; char name[UMI_FINANCIAL_CORE_NAME_CAPACITY]; UmiFinancialDate effective_date; uint32_t state; bool active; } UmiSettlementInstruction;
/* Initialize the typed financial record. */ UmiStatus umi_settlement_instruction_init(UmiSettlementInstruction *item,const char *id,const char *name,const char *parent_id,UmiFinancialDate effective_date,uint32_t state);
/* Validate the typed financial record. */ bool umi_settlement_instruction_is_valid(const UmiSettlementInstruction *item);
#ifdef __cplusplus
}
#endif

#endif
