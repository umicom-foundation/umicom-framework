/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_book_registry.c
 *
 * PURPOSE:
 *   Exercise the book registry financial-core contract.
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
#include "umicom/finance/core/book_registry.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiBookRegistry r; UmiFinancialBook x; umi_book_registry_init(&r); CHECK(umi_book_init(&x,"B","Book","LE")==UMI_STATUS_OK); CHECK(umi_book_registry_add(&r,&x)==UMI_STATUS_OK);
    return 0;
}
