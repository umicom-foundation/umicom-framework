/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_fixing_store.c
 *
 * PURPOSE:
 *   Exercise the fixing store financial-core contract.
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
#include "umicom/finance/core/fixing_store.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiFixingStore s; umi_fixing_store_init(&s); CHECK(umi_fixing_store_count(&s)==0U);
    return 0;
}
