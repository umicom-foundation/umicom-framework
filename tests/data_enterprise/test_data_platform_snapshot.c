/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_data_platform_snapshot.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the data platform snapshot enterprise data capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/data_platform_snapshot.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiDataPlatformSnapshot s; UmiDataEntityRegistry e; UmiDataConnectionPool p; UmiDataReplicaSet r; UmiDataOperationQueue q; umi_data_entity_registry_init(&e); umi_data_connection_pool_init(&p); umi_data_replica_set_init(&r); umi_data_data_operation_queue_init(&q); CHECK(umi_data_data_platform_snapshot_build(&s,11U,&e,&p,&r,&q,UMI_DATA_ENTERPRISE_READY,5U)==UMI_STATUS_OK); CHECK(s.schema_fingerprint==11U && s.revision==5U);
    return 0;
}
