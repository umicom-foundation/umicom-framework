/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_service_lease.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the service lease Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/service_lease.h"
#include "../value_archive/construction_cases.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted service lease value. */
static int UmiFabricServiceLeaseConstructionCases(void)
{
    UmiFabricServiceLease *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_fabric_service_lease_init(value, "risk.1", 100U, 50U) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->instance_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_fabric_service_lease_init(value, oversized, 100U, 50U) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_fabric_service_lease_init(value, value->instance_id, 100U, 50U) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->instance_id, "risk.1") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiFabricServiceLeaseConstructionCases() != 0) return 1;

    UmiFabricServiceLease l; CHECK(umi_fabric_service_lease_init(&l,"risk.1",100U,50U)==UMI_STATUS_OK);
    CHECK(umi_fabric_service_lease_active(&l,149U)); CHECK(!umi_fabric_service_lease_active(&l,150U));
    CHECK(umi_fabric_service_lease_renew(&l,200U,20U)==UMI_STATUS_OK); CHECK(l.renewals==1U);
    return 0;
}
