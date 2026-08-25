/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_replication_lag.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the replication lag enterprise data capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/replication_lag.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

int main(void) {
    UmiDataReplicationLag l; CHECK(umi_data_replication_lag_compute(&l,"r1",20U,1U,100U,500U)==UMI_STATUS_OK); CHECK(l.state==UMI_DATA_ENTERPRISE_READY); CHECK(umi_data_replication_lag_compute(&l,"r1",600U,5U,100U,500U)==UMI_STATUS_OK); CHECK(l.state==UMI_DATA_ENTERPRISE_FAILED);
    return 0;
}
