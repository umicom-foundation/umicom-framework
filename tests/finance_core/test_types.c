/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_types.c
 *
 * PURPOSE:
 *   Exercise the types financial-core contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return 1; } while (0)
#include <string.h>
#include "umicom/finance/core/types.h"

int main(void)
{
    UmiFinancialId id= {0}; CHECK(umi_financial_id_assign(&id,"T1")==UMI_STATUS_OK); CHECK(umi_financial_id_is_valid(&id)); CHECK(umi_financial_date_is_valid((UmiFinancialDate){2028,2U,29U}));
    return 0;
}
