/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_book.c
 *
 * PURPOSE:
 *   Exercise the book financial-core contract.
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
#include "umicom/finance/core/book.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiFinancialBook x; CHECK(umi_book_init(&x,"ID","Name","PARENT")==UMI_STATUS_OK); CHECK(umi_book_is_valid(&x));
    return 0;
}
