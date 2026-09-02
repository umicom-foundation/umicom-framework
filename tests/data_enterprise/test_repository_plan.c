/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_repository_plan.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the repository plan enterprise data capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/repository_plan.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiDataChangeTracker t; UmiDataRepositoryPlan p; umi_data_change_tracker_init(&t); CHECK(umi_data_change_tracker_track(&t,"c1","Order","42",UMI_DATA_CHANGE_UPDATE)==UMI_STATUS_OK); CHECK(umi_data_repository_plan_build(&p,&t)==UMI_STATUS_OK); CHECK(umi_data_repository_plan_validate(&p)==UMI_STATUS_OK); CHECK(p.items[0].kind==UMI_DATA_CHANGE_UPDATE);
    return 0;
}
