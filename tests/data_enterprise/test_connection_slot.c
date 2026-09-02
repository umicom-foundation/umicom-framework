/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_connection_slot.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the connection slot enterprise data capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/connection_slot.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiDataConnectionSlot item;
    CHECK(umi_data_connection_slot_init(&item,"slot1",100U) == UMI_STATUS_OK);
    CHECK(item.healthy && !item.leased);
    return 0;
}
