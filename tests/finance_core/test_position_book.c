/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_position_book.c
 *
 * PURPOSE:
 *   Exercise the position book financial-core contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return 1; } while (0)
#include <string.h>
#include "umicom/finance/core/position_book.h"

int main(void)
{
    UmiPositionBook b; UmiPositionKey k; UmiPosition p; umi_position_book_init(&b); CHECK(umi_position_key_init(&k,"B","P","X")==UMI_STATUS_OK); CHECK(umi_position_init(&p,&k,(UmiQuantity){1.0},(UmiFinancialDate){2026,8U,25U})==UMI_STATUS_OK); CHECK(umi_position_book_add(&b,&p)==UMI_STATUS_OK);
    return 0;
}
