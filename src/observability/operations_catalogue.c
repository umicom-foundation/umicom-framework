/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/observability/operations_catalogue.c
 *
 * PURPOSE:
 *   Implement the operations catalogue behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Operations view catalogue | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/observability/operations_catalogue.h"
#include <string.h>
static const UmiOperationsViewDescriptor VIEWS[] = {
    { UMI_OPERATIONS_VIEW_OVERVIEW,"overview","Operations Overview","Runtime health, evidence, resources and resilience" },
    { UMI_OPERATIONS_VIEW_METRICS,"metrics","Metrics","Counters, gauges and current resource samples" },
    { UMI_OPERATIONS_VIEW_TRACES,"traces","Traces","Correlated operation and service spans" },
    { UMI_OPERATIONS_VIEW_PROFILER,"profiler","Profiler","CPU, memory, process and task profiling" },
    { UMI_OPERATIONS_VIEW_BENCHMARKS,"benchmarks","Benchmarks","Performance history and regression evidence" },
    { UMI_OPERATIONS_VIEW_CRASHES,"crashes","Crash Reports","Crash evidence, recovery and report references" },
    { UMI_OPERATIONS_VIEW_HEALTH,"health","Health","Readiness, liveness and component health" },
    { UMI_OPERATIONS_VIEW_RESILIENCE,"resilience","Resilience","Retry, circuit, rate limit and supervisor state" },
    { UMI_OPERATIONS_VIEW_EVENTS,"events","Events","Operational lifecycle and recovery events" },
    { UMI_OPERATIONS_VIEW_AUDIT,"audit","Audit","Consequential action and policy evidence" }
};
/*
 * Return the number of records represented by operations catalogue without changing their
 * state.
 */
size_t umi_operations_catalogue_count(void) { return sizeof(VIEWS) / sizeof(VIEWS[0]); }
/*
 * Find operations catalogue while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiOperationsViewDescriptor *umi_operations_catalogue_at(size_t index) { return index < umi_operations_catalogue_count() ? &VIEWS[index] : NULL; }
/*
 * Find operations catalogue while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiOperationsViewDescriptor *umi_operations_catalogue_find(const char *id) { size_t index; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (id == NULL) return NULL; /* Visit each bounded item once so every record receives the same rule. */ for (index = 0U; index < umi_operations_catalogue_count(); ++index) /* Protect caller-owned memory by checking that required state is available before it is used. */ if (strcmp(VIEWS[index].id,id) == 0) return &VIEWS[index]; return NULL; }
