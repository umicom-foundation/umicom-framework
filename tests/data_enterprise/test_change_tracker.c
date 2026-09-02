/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_change_tracker.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the change tracker enterprise data capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/change_tracker.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiDataChangeTracker t; umi_data_change_tracker_init(&t); CHECK(umi_data_change_tracker_track(&t,"c1","Order","42",UMI_DATA_CHANGE_INSERT)==UMI_STATUS_OK); CHECK(umi_data_change_tracker_count_kind(&t,UMI_DATA_CHANGE_INSERT)==1U); CHECK(umi_data_change_tracker_track(&t,"c2","Order","42",UMI_DATA_CHANGE_UPDATE)==UMI_STATUS_OK); CHECK(umi_data_change_tracker_count_kind(&t,UMI_DATA_CHANGE_UPDATE)==1U);
    return 0;
}
