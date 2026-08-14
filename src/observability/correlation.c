/* Umicom Framework | Operational correlation | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/observability/correlation.h"
#include <string.h>

UmiStatus umi_operations_correlate(uint64_t correlation_id,const UmiTraceStore *traces,const UmiAuditLog *audit,const UmiOperationalEventLog *events,UmiOperationsCorrelationSummary *out_summary)
{
    size_t index;
    if (correlation_id == 0U || traces == NULL || audit == NULL || events == NULL || out_summary == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_summary,0,sizeof(*out_summary));
    out_summary->correlation_id = correlation_id;
    for (index = 0U; index < umi_trace_store_count(traces); ++index) {
        UmiTraceSpan span;
        if (umi_trace_store_at(traces,index,&span) == UMI_STATUS_OK && span.trace_id == correlation_id) { out_summary->spans += 1U; if (span.status == UMI_TRACE_ERROR) out_summary->failed_spans += 1U; }
    }
    for (index = 0U; index < umi_audit_log_count(audit); ++index) {
        UmiAuditRecord record;
        if (umi_audit_log_at(audit,index,&record) == UMI_STATUS_OK && record.correlation_id == correlation_id) { out_summary->audit_records += 1U; if (record.outcome != UMI_AUDIT_SUCCEEDED) out_summary->denied_or_failed_audits += 1U; }
    }
    for (index = 0U; index < umi_operational_event_log_count(events); ++index) {
        UmiOperationalEvent event;
        if (umi_operational_event_log_at(events,index,&event) == UMI_STATUS_OK && event.correlation_id == correlation_id) { out_summary->operational_events += 1U; if (event.severity >= UMI_DIAGNOSTIC_ERROR) out_summary->error_events += 1U; }
    }
    return UMI_STATUS_OK;
}
