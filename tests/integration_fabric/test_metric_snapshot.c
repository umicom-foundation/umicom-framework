/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_metric_snapshot.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the metric snapshot Integration Fabric capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/fabric/metric_snapshot.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

int main(void) {
    UmiFabricMetricSnapshot s; umi_fabric_metric_snapshot_init(&s); umi_fabric_metric_snapshot_record(&s,false,false,10U); umi_fabric_metric_snapshot_record(&s,true,true,30U); CHECK(s.errors==1U&&s.retries==1U); CHECK(umi_fabric_metric_snapshot_average_latency(&s)==20.0);
    return 0;
}
