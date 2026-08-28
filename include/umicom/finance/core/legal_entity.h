/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/legal_entity.h
 *
 * PURPOSE:
 *   Define legal entities shared by all financial applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_LEGAL_ENTITY_H
#define UMICOM_FINANCE_CORE_LEGAL_ENTITY_H

#include "umicom/finance/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiLegalEntity { UmiFinancialId entity_id; char name[UMI_FINANCIAL_CORE_NAME_CAPACITY]; char code[UMI_FINANCIAL_CORE_CODE_CAPACITY]; bool active; } UmiLegalEntity;
/* Initialize the typed financial record. */ UmiStatus umi_legal_entity_init(UmiLegalEntity *item,const char *id,const char *name,const char *code);
/* Validate the typed financial record. */ bool umi_legal_entity_is_valid(const UmiLegalEntity *item);
#ifdef __cplusplus
}
#endif

#endif
