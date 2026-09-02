/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_replica_descriptor.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the replica descriptor enterprise data capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/replica_descriptor.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiDataReplicaDescriptor item;
    CHECK(umi_data_replica_descriptor_init(&item,"r1","db://primary",1U,true) == UMI_STATUS_OK);
    CHECK(item.primary && item.writable);
    return 0;
}
