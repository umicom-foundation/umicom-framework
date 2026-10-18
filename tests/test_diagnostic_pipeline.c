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
/* Required fixture setup and checks also execute in optimized builds. */
#include "support/assertions.h"
#include <assert.h>
#include <string.h>
#include "umicom/diagnostics/log.h"
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

    /* Exercise the installed parser registry, not only the runtime parser in
     * isolation. Toolkit warnings remain warnings in the shared Problems model. */
    assert(umi_diagnostic_pipeline_ingest_line(pipeline, "native", "Native", "gtk",
        UMI_OUTPUT_STREAM_ERROR, "Gtk-WARNING **: settings changed", 10U) == UMI_STATUS_OK);
    assert(umi_diagnostic_pipeline_ingest_line(pipeline, "native", "Native", "gtk",
        UMI_OUTPUT_STREAM_ERROR, "GLib-GIO-CRITICAL **: invalid model index", 11U) == UMI_STATUS_OK);
    assert(umi_diagnostic_pipeline_snapshot(pipeline, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.lines_ingested == 3U && snapshot.parse_matches == 3U);
    assert(snapshot.diagnostics.retained_count == 3U);
    UmiDiagnosticSnapshot item;
    assert(umi_diagnostic_model_at(umi_diagnostic_pipeline_model(pipeline), 1U, &item) == UMI_STATUS_OK);
    assert(item.severity == UMI_DIAGNOSTIC_WARNING && item.correlation_id == 10U);
    assert(strcmp(item.code, "native-toolkit-warning") == 0);
    assert(umi_diagnostic_model_at(umi_diagnostic_pipeline_model(pipeline), 2U, &item) == UMI_STATUS_OK);
    assert(item.severity == UMI_DIAGNOSTIC_ERROR && item.correlation_id == 11U);
    assert(strcmp(item.code, "native-toolkit-critical") == 0);

    /* Applications can report their startup stage through the same sink while
     * preserving the original failure status in the displayed message. */
    UmiLogger logger = {umi_diagnostic_pipeline_sink, pipeline, "startup"};
    umi_log_status(&logger, "adapter.prepare", UMI_STATUS_INVALID_STATE, 12U);
    assert(umi_diagnostic_model_at(umi_diagnostic_pipeline_model(pipeline), 3U, &item) == UMI_STATUS_OK);
    assert(item.severity == UMI_DIAGNOSTIC_ERROR && item.correlation_id == 12U);
    assert(strcmp(item.source, "startup") == 0);
    assert(strcmp(item.message, "Operation adapter.prepare failed: Invalid state (2).") == 0);
    umi_diagnostic_pipeline_destroy(pipeline);
    return 0;
}
