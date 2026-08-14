/* Umicom Framework | Operations correlation tests | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "umicom/observability/correlation.h"
int main(void)
{
    UmiTraceStore *traces = NULL;
    UmiAuditLog *audit = NULL;
    UmiOperationalEventLog *events = NULL;
    UmiAuditRecord record = {0};
    UmiOperationalEvent event = {0};
    UmiOperationsCorrelationSummary summary;
    uint64_t span_id = 0U;
    assert(umi_trace_store_create(&traces) == UMI_STATUS_OK);
    assert(umi_audit_log_create(&audit) == UMI_STATUS_OK);
    assert(umi_operational_event_log_create(&events) == UMI_STATUS_OK);
    assert(umi_trace_span_begin(traces,57U,0U,"studio.build",1U,&span_id) == UMI_STATUS_OK);
    assert(umi_trace_span_end(traces,span_id,UMI_TRACE_ERROR,2U) == UMI_STATUS_OK);
    record.correlation_id = 57U; record.outcome = UMI_AUDIT_DENIED;
    (void)strcpy(record.principal,"developer"); (void)strcpy(record.action,"build"); (void)strcpy(record.resource,"project");
    assert(umi_audit_log_append(audit,&record) == UMI_STATUS_OK);
    event.correlation_id = 57U; event.severity = UMI_DIAGNOSTIC_ERROR;
    (void)strcpy(event.category,"build"); (void)strcpy(event.message,"compiler failed");
    assert(umi_operational_event_log_append(events,&event) == UMI_STATUS_OK);
    assert(umi_operations_correlate(57U,traces,audit,events,&summary) == UMI_STATUS_OK);
    assert(summary.spans == 1U && summary.failed_spans == 1U);
    assert(summary.audit_records == 1U && summary.denied_or_failed_audits == 1U);
    assert(summary.operational_events == 1U && summary.error_events == 1U);
    umi_operational_event_log_destroy(events); umi_audit_log_destroy(audit); umi_trace_store_destroy(traces);
    return 0;
}
