/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/identifier.c
 *
 * PURPOSE:
 *   Validate and compare stable financial identifiers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The implementation is deliberately small and deterministic so callers can test identifier behaviour without starting a complete product.
 */

#include <string.h>
#include "umicom/finance/identifier.h"
/* Check that financial id satisfies its contract before another service relies on it. */
int umi_financial_id_valid(const UmiFinancialId *id){return id!=NULL && id->value[0]!='\0' && strlen(id->value)<UMI_FINANCE_ID_CAPACITY;}
/*
 * Provide the financial id equal operation used by this module and its client
 * applications.
 */
int umi_financial_id_equal(const UmiFinancialId *left,const UmiFinancialId *right){return left!=NULL && right!=NULL && strcmp(left->value,right->value)==0;}
