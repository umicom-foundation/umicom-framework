/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_savepoint_plan.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the savepoint plan enterprise data capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/savepoint_plan.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void) {
    UmiDataSavepointPlan item;
    CHECK(umi_data_savepoint_plan_init(&item,"sp1",1U) == UMI_STATUS_OK);
    CHECK(item.release_on_success && item.rollback_on_failure);
    return 0;
}
