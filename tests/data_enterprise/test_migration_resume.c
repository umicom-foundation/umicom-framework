/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_migration_resume.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the migration resume enterprise data capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/migration_resume.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void) {
    UmiDataMigrationPlan p={0}; UmiDataMigrationCheckpoint c; size_t next=0U; p.count=3U; CHECK(umi_data_migration_checkpoint_init(&c,"cp","m",1U,10U,20U)==UMI_STATUS_OK); CHECK(umi_data_migration_resume_decide(&p,&c,20U,&next)==UMI_STATUS_OK); CHECK(next==1U); CHECK(umi_data_migration_resume_decide(&p,&c,21U,&next)==UMI_STATUS_INVALID_STATE);
    return 0;
}
