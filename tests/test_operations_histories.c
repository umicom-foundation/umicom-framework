/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_operations_histories.c
 *
 * PURPOSE:
 *   Implement the test operations histories behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Operations history tests | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "umicom/observability/observability_admin.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiOperationsResourceHistory resources = {0};
    UmiOperationsBenchmarkHistory benchmarks = {0};
    UmiOperationsCrashRegistry crashes = {0};
    UmiOperationsResourceSample first = {"studio",1U,18.5,1000U,4U,8U,2U};
    UmiOperationsResourceSample second = {"studio",2U,26.0,1200U,5U,9U,3U};
    UmiOperationsBenchmarkRun baseline = {"parse","project-a","run-1",1U,1000U,500.0,true};
    UmiOperationsBenchmarkRun current = {"parse","project-a","run-2",2U,800U,625.0,true};
    UmiOperationsBenchmarkComparison comparison;
    UmiOperationsCrashEvidence crash = {"crash-1","studio","parser","Unexpected input","reports/crash-1",3U,42U,false};
    assert(umi_operations_resource_history_add(&resources,&first) == UMI_STATUS_OK);
    assert(umi_operations_resource_history_add(&resources,&second) == UMI_STATUS_OK);
    assert(umi_operations_resource_history_peak_cpu(&resources,"studio") == 26.0);
    assert(umi_operations_resource_history_peak_memory(&resources,"studio") == 1200U);
    assert(umi_operations_resource_history_latest(&resources,"studio")->timestamp_ns == 2U);
    assert(umi_operations_benchmark_history_add(&benchmarks,&baseline) == UMI_STATUS_OK);
    assert(umi_operations_benchmark_history_add(&benchmarks,&current) == UMI_STATUS_OK);
    assert(umi_operations_benchmark_compare(&baseline,&current,2.0,&comparison) == UMI_STATUS_OK);
    assert(comparison.trend == UMI_OPERATIONS_TREND_IMPROVED);
    assert(umi_operations_crash_registry_add(&crashes,&crash) == UMI_STATUS_OK);
    assert(umi_operations_crash_registry_unrecovered(&crashes) == 1U);
    assert(umi_operations_crash_registry_mark_recovered(&crashes,"crash-1") == UMI_STATUS_OK);
    assert(umi_operations_crash_registry_unrecovered(&crashes) == 0U);
    return 0;
}
