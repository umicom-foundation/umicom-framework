/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_unit_of_work.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the unit of work enterprise data capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/unit_of_work.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void) {
    UmiDataUnitOfWork u; CHECK(umi_data_unit_of_work_begin(&u,"uow-1")==UMI_STATUS_OK); CHECK(umi_data_unit_of_work_commit(&u)==UMI_STATUS_INVALID_STATE); CHECK(umi_data_unit_of_work_validate(&u,true)==UMI_STATUS_OK); CHECK(umi_data_unit_of_work_commit(&u)==UMI_STATUS_OK); CHECK(u.committed);
    return 0;
}
