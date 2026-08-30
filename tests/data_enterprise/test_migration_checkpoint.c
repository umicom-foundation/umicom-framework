/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_migration_checkpoint.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the migration checkpoint enterprise data capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/migration_checkpoint.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void) {
    UmiDataMigrationCheckpoint item;
    CHECK(umi_data_migration_checkpoint_init(&item,"cp1","mig1",2U,11U,22U) == UMI_STATUS_OK);
    CHECK(item.completed_steps==2U);
    return 0;
}
