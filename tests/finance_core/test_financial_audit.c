/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_financial_audit.c
 *
 * PURPOSE:
 *   Exercise the financial audit financial-core contract.
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
#include "umicom/finance/core/financial_audit.h"

int main(void)
{
    UmiFinancialAuditRecord x; CHECK(umi_financial_audit_init(&x,"ID","Name","PARENT",(UmiFinancialDate){2026,8U,25U},1U)==UMI_STATUS_OK); CHECK(umi_financial_audit_is_valid(&x));
    return 0;
}
