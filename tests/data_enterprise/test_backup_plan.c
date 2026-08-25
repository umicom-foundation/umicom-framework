/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_backup_plan.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the backup plan enterprise data capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/backup_plan.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void) {
    UmiDataBackupPlan item;
    CHECK(umi_data_backup_plan_init(&item,"b1","backup://daily",10U,false) == UMI_STATUS_OK);
    CHECK(item.encrypted);
    return 0;
}
