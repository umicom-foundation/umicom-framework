/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/banking/bank_relationship.h
 *
 * PURPOSE:
 *   Represent customer-to-bank relationship ownership independent of presentation channels.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_BANKING_BANK_RELATIONSHIP_H
#define UMICOM_FINANCE_BANKING_BANK_RELATIONSHIP_H
#include "umicom/finance/banking/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiBankingBankRelationship {
    UmiFinancialId id;
    UmiFinancialId customer_id;
    char relationship_manager[UMI_BANKING_NAME_CAPACITY];
    bool primary_relationship;
} UmiBankingBankRelationship;
UmiStatus umi_banking_bank_relationship_init(UmiBankingBankRelationship *value,
    const char *id,
    const char *customer_id,
    const char *relationship_manager,
    bool primary_relationship);
bool umi_banking_bank_relationship_valid(const UmiBankingBankRelationship *value);
bool umi_banking_bank_relationship_primary(const UmiBankingBankRelationship *value);
#ifdef __cplusplus
}
#endif
#endif
