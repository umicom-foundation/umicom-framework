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
/*
 * Search only inside the actual fixed-size field. strlen would already read
 * beyond an unterminated identifier before its returned length could be tested.
 * Comparisons below validate both operands before using C string operations.
 */
int umi_financial_id_valid(const UmiFinancialId *id){return id!=NULL && id->value[0]!='\0' && memchr(id->value, '\0', sizeof(id->value)) != NULL;}
/*
 * Provide the financial id equal operation used by this module and its client
 * applications.
 */
int umi_financial_id_equal(const UmiFinancialId *left,const UmiFinancialId *right){return umi_financial_id_valid(left) && umi_financial_id_valid(right) && strcmp(left->value,right->value)==0;}
