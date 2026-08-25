#include <stdio.h>
#include "umicom/observability/performance/timeline.h"

int main(void) {
    UmiPerformanceTimeline left;
    UmiPerformanceTimeline right;
    if (umi_performance_timeline_init(&left, "timeline", "framework") != UMI_STATUS_OK) return 1;
    if (umi_performance_timeline_validate(&left) != UMI_STATUS_OK) return 2;
    if (umi_performance_timeline_observe(&left, 12.0, 3.0, 4U, 100U) != UMI_STATUS_OK || left.sequence != 1U) return 3;
    if (umi_performance_timeline_weighted_score(10.0, 20.0, 0.5) != 20.0) return 4;
    if (umi_performance_timeline_init(&right, "timeline", "framework") != UMI_STATUS_OK) return 5;
    if (!umi_performance_timeline_same_identity(&left, &right)) return 6;
    puts("timeline: ok");
    return 0;
}
