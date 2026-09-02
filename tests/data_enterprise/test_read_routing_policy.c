/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_read_routing_policy.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the read routing policy enterprise data capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/read_routing_policy.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiDataReadRoutingPolicy p; UmiDataReplicaSet s; UmiDataReplicaDescriptor r; UmiDataReplicationLag lag; size_t index=99U; umi_data_read_routing_policy_default(&p); umi_data_replica_set_init(&s); CHECK(umi_data_replica_descriptor_init(&r,"primary","db://p",2U,true)==UMI_STATUS_OK); CHECK(umi_data_replica_set_add(&s,&r)==UMI_STATUS_OK); CHECK(umi_data_replica_descriptor_init(&r,"replica","db://r",1U,false)==UMI_STATUS_OK); CHECK(umi_data_replica_set_add(&s,&r)==UMI_STATUS_OK); CHECK(umi_data_replication_lag_compute(&lag,"replica",10U,0U,100U,500U)==UMI_STATUS_OK); CHECK(umi_data_read_routing_policy_select(&p,&s,&lag,1U,&index)==UMI_STATUS_OK); CHECK(index==1U);
    return 0;
}
