/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_diagnostic_pipeline.c
 *
 * PURPOSE:
 *   Implement the test diagnostic pipeline behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/diagnostics/pipeline.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDiagnosticPipeline *pipeline = NULL;
    UmiDiagnosticPipelineSnapshot snapshot;
    assert(umi_diagnostic_pipeline_create(NULL, &pipeline) == UMI_STATUS_OK);
    assert(umi_diagnostic_pipeline_ingest_line(pipeline, "build", "Build", "gcc",
        UMI_OUTPUT_STREAM_ERROR, "src/a.c:4:2: warning: unused value", 9U) == UMI_STATUS_OK);
    assert(umi_diagnostic_pipeline_snapshot(pipeline, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.lines_ingested == 1U && snapshot.parse_matches == 1U);
    assert(snapshot.diagnostics.retained_count == 1U);
    assert(snapshot.output.retained_count == 2U);
    umi_diagnostic_pipeline_destroy(pipeline);
    return 0;
}
