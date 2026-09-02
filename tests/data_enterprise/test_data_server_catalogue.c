/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_data_server_catalogue.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the data server catalogue enterprise data capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/data_server_catalogue.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiDataServerCatalogue registry;
    UmiDataServerProfile item;
    UmiDataServerProfile found;
    umi_data_data_server_catalogue_init(&registry);
    CHECK(umi_data_data_server_profile_init(&item,"production",2U,8U,1000U) == UMI_STATUS_OK);
    CHECK(umi_data_data_server_catalogue_add(&registry, &item) == UMI_STATUS_OK);
    CHECK(umi_data_data_server_catalogue_add(&registry, &item) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(umi_data_data_server_catalogue_find(&registry, item.profile_id, &found) == UMI_STATUS_OK);
    CHECK(strcmp(found.profile_id, item.profile_id) == 0);
    CHECK(umi_data_data_server_catalogue_count(&registry) == 1U);
    return 0;
}
