/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_transaction_scope.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the transaction scope enterprise data capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/transaction_scope.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiDataTransactionScope s; CHECK(umi_data_transaction_scope_begin(&s,"tx1")==UMI_STATUS_OK); CHECK(umi_data_transaction_scope_enter(&s)==UMI_STATUS_OK); CHECK(umi_data_transaction_scope_leave(&s,true)==UMI_STATUS_OK); CHECK(umi_data_transaction_scope_leave(&s,true)==UMI_STATUS_OK); CHECK(s.committed);
    return 0;
}
