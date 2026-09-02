/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_replica_set.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the replica set enterprise data capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/replica_set.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiDataReplicaSet registry;
    UmiDataReplicaDescriptor item;
    UmiDataReplicaDescriptor found;
    umi_data_replica_set_init(&registry);
    CHECK(umi_data_replica_descriptor_init(&item,"r1","db://primary",1U,true) == UMI_STATUS_OK);
    CHECK(umi_data_replica_set_add(&registry, &item) == UMI_STATUS_OK);
    CHECK(umi_data_replica_set_add(&registry, &item) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(umi_data_replica_set_find(&registry, item.replica_id, &found) == UMI_STATUS_OK);
    CHECK(strcmp(found.replica_id, item.replica_id) == 0);
    CHECK(umi_data_replica_set_count(&registry) == 1U);
    return 0;
}
