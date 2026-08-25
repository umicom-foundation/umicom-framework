/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_restore_plan.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the restore plan enterprise data capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/restore_plan.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void) {
    UmiDataRestorePlan item;
    CHECK(umi_data_restore_plan_init(&item,"r1","b1",11U,true) == UMI_STATUS_OK);
    CHECK(item.verify_only && item.preserve_existing);
    return 0;
}
