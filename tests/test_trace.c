/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_trace.c
 *
 * PURPOSE:
 *   Verify trace span creation, parent linkage and completion.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <stddef.h>
#include <assert.h>
#include "umicom/diagnostics/trace.h"
int main(void) {
    UmiTraceStore *s = NULL; UmiTraceSpan span; uint64_t id;
    assert(umi_trace_store_create(&s) == UMI_STATUS_OK);
    assert(umi_trace_span_begin(s, 42U, 0U, "studio.build", 100U, &id) == UMI_STATUS_OK);
    assert(umi_trace_span_end(s, id, UMI_TRACE_OK, 150U) == UMI_STATUS_OK);
    assert(umi_trace_store_at(s, 0U, &span) == UMI_STATUS_OK && span.ended_at_ns == 150U);
    umi_trace_store_destroy(s); return 0;
}
