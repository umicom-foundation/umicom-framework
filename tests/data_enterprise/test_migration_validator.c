/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_migration_validator.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the migration validator enterprise data capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/migration_validator.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiDataMigrationPlan p={0}; UmiDataMigrationValidation v; p.count=1U; p.destructive=true; CHECK(umi_data_migration_validator_check(&p,UMI_DATA_BREAKING,false,false,&v)==UMI_STATUS_OK); CHECK(!v.valid); CHECK(v.blockers==2U);
    return 0;
}
