/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_profiler.c
 *
 * PURPOSE:
 *   Verify profile timing aggregates count, total, minimum and maximum
 *   durations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <stddef.h>
#include <assert.h>
#include "umicom/diagnostics/profiler.h"
int main(void) {
    UmiProfiler *p = NULL; UmiProfileScope s; UmiProfileSample sample;
    assert(umi_profiler_create(&p) == UMI_STATUS_OK);
    assert(umi_profiler_begin("index", 100U, &s) == UMI_STATUS_OK);
    assert(umi_profiler_end(p, &s, 150U) == UMI_STATUS_OK);
    assert(umi_profiler_begin("index", 200U, &s) == UMI_STATUS_OK);
    assert(umi_profiler_end(p, &s, 280U) == UMI_STATUS_OK);
    assert(umi_profiler_at(p, 0U, &sample) == UMI_STATUS_OK && sample.count == 2U && sample.total_ns == 130U);
    umi_profiler_destroy(p); return 0;
}
