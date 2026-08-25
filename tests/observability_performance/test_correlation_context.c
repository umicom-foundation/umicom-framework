#include <stdio.h>
#include "umicom/observability/performance/correlation_context.h"

int main(void) {
    UmiPerformanceCorrelationContext left;
    UmiPerformanceCorrelationContext right;
    if (umi_performance_correlation_context_init(&left, "correlation_context", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_correlation_context_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_correlation_context_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (umi_performance_correlation_context_fingerprint("evidence") == 0U) return 4;
    if (umi_performance_correlation_context_init(&right, "correlation_context", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_correlation_context_same_identity(&left, &right)) return 6;
    puts("correlation_context: ok");
    return 0;
}
