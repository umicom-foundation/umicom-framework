/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_pool_health.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the pool health enterprise data capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/pool_health.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiDataConnectionPool p; UmiDataConnectionSlot s; UmiDataPoolHealth h; umi_data_connection_pool_init(&p); CHECK(umi_data_connection_slot_init(&s,"slot1",1U)==UMI_STATUS_OK); CHECK(umi_data_connection_pool_add(&p,&s)==UMI_STATUS_OK); CHECK(umi_data_pool_health_compute(&p,&h)==UMI_STATUS_OK); CHECK(h.available==1U); CHECK(h.state==UMI_DATA_ENTERPRISE_READY);
    return 0;
}
