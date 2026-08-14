/* Umicom Framework | Health and readiness summary | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/observability/health_summary.h"
#include <string.h>

UmiStatus umi_operations_health_summarise(const UmiHealthRegistry *health,const UmiReadinessRegistry *readiness,UmiOperationsHealthSummary *out_summary)
{
    size_t index;
    if (health == NULL || readiness == NULL || out_summary == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_summary,0,sizeof(*out_summary));
    out_summary->components = umi_health_registry_count(health);
    out_summary->readiness_checks = umi_readiness_registry_count(readiness);
    out_summary->overall = out_summary->components == 0U ? UMI_OPERATIONS_HEALTH_UNKNOWN : UMI_OPERATIONS_HEALTH_READY;
    for (index = 0U; index < out_summary->components; ++index) {
        UmiHealthSnapshot item;
        if (umi_health_registry_at(health,index,&item) != UMI_STATUS_OK) continue;
        if (item.state == UMI_HEALTH_READY) out_summary->ready_components += 1U;
        else if (item.state == UMI_HEALTH_FAILED) out_summary->failed_components += 1U;
        else out_summary->degraded_components += 1U;
    }
    for (index = 0U; index < out_summary->readiness_checks; ++index) { UmiReadinessCheck check; if (umi_readiness_registry_at(readiness,index,&check) == UMI_STATUS_OK && !check.ready) out_summary->failed_readiness_checks += 1U; }
    if (out_summary->failed_components > 0U) out_summary->overall = UMI_OPERATIONS_HEALTH_FAILED;
    else if (out_summary->degraded_components > 0U || out_summary->failed_readiness_checks > 0U) out_summary->overall = UMI_OPERATIONS_HEALTH_DEGRADED;
    return UMI_STATUS_OK;
}
