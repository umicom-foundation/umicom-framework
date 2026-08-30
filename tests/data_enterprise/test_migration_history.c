/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_migration_history.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the migration history enterprise data capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/migration_history.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void) {
    UmiDataMigrationHistory h; uint64_t fp=0U; umi_data_migration_history_init(&h); CHECK(umi_data_migration_history_append(&h,"m1",10U,99U,true)==UMI_STATUS_OK); CHECK(umi_data_migration_history_latest_success(&h,&fp)==UMI_STATUS_OK); CHECK(fp==99U);
    return 0;
}
