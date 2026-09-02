/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_data_session.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the data session enterprise data capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/data_session.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiDataSession item;
    CHECK(umi_data_data_session_init(&item,"s1","user",UMI_DATA_CONSISTENCY_SESSION,10U) == UMI_STATUS_OK);
    CHECK(item.last_activity==10U);
    return 0;
}
