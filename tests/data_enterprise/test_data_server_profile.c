/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_data_server_profile.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the data server profile enterprise data capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/data_server_profile.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void) {
    UmiDataServerProfile item;
    CHECK(umi_data_data_server_profile_init(&item,"production",2U,16U,10000U) == UMI_STATUS_OK);
    CHECK(item.migrations_enabled);
    return 0;
}
