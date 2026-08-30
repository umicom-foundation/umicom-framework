/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_diagnostic_build_projection.c
 *
 * PURPOSE:
 *   Implement the test diagnostic build projection behavior for
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
#include <string.h>
#include "umicom/diagnostic_ui/build_projection.h"
int main(void)
{
    UmiDiagnosticPipeline *pipeline = NULL;
    UmiBuildResult result;
    UmiBuildDiagnostic diagnostic = {0};
    UmiDiagnosticPipelineSnapshot snapshot;
    assert(umi_diagnostic_pipeline_create(NULL, &pipeline) == UMI_STATUS_OK);
    umi_build_result_init(&result, 42U, UMI_BUILD_PHASE_BUILD, "debug");
    (void)strcpy(result.output, "compile failed");
    (void)strcpy(diagnostic.file, "src/a.c");
    (void)strcpy(diagnostic.message, "bad token");
    diagnostic.severity = UMI_BUILD_DIAGNOSTIC_ERROR;
    assert(umi_build_diagnostic_list_add(&result.diagnostics, &diagnostic) == UMI_STATUS_OK);
    assert(umi_diagnostic_build_result_ingest(pipeline, &result, "build") == UMI_STATUS_OK);
    assert(umi_diagnostic_pipeline_snapshot(pipeline, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.diagnostics.retained_count == 1U && snapshot.output.retained_count >= 1U);
    umi_diagnostic_pipeline_destroy(pipeline);
    return 0;
}
