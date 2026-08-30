/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_data_health.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the data health enterprise data capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/data_health.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void) {
    UmiDataHealth h; CHECK(umi_data_health_compute(&h,UMI_DATA_ENTERPRISE_READY,UMI_DATA_ENTERPRISE_DEGRADED,UMI_DATA_ENTERPRISE_READY,false)==UMI_STATUS_OK); CHECK(h.overall_state==UMI_DATA_ENTERPRISE_DEGRADED);
    return 0;
}
