#include <stdio.h>
#include "umicom/observability/performance/counter.h"

int main(void) {
    UmiPerformanceCounter left;
    UmiPerformanceCounter right;
    if (umi_performance_counter_init(&left, "counter", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_counter_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_counter_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (!umi_performance_counter_exceeds(11.0, 10.0)) return 4;
    if (umi_performance_counter_init(&right, "counter", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_counter_same_identity(&left, &right)) return 6;
    puts("counter: ok");
    return 0;
}
