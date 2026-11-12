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
/* Required fixture setup and checks also execute in optimized builds. */
#include "support/assertions.h"
#include <assert.h>
#include <string.h>
#include "umicom/diagnostic_ui/build_projection.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
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
    /* A refused replacement must not resolve the previous producer's error or
     * append output. Exercise admission before those two model mutations. */
    size_t retainedOutput=snapshot.output.retained_count;
    result.operation_id=43U;
    for (unsigned field=0;field<3U;++field) {
        char *text=field==0U?result.profile_id:field==1U?result.command:result.diagnostics.items[0].message;
        size_t capacity=field==0U?sizeof result.profile_id:field==1U?sizeof result.command:sizeof result.diagnostics.items[0].message;
        memset(text,'x',capacity);
        assert(umi_diagnostic_build_result_ingest(pipeline,&result,"build")==UMI_STATUS_INVALID_ARGUMENT);
        assert(umi_diagnostic_pipeline_snapshot(pipeline,&snapshot)==UMI_STATUS_OK);
        assert(snapshot.diagnostics.retained_count==1U&&snapshot.output.retained_count==retainedOutput);
        UmiDiagnosticSnapshot original;
        assert(umi_diagnostic_model_find(umi_diagnostic_pipeline_model(pipeline),"build-42-0",&original)==UMI_STATUS_OK);
        assert(!original.resolved&&original.correlation_id==42U);
        memset(text,0,capacity);
        if (field==0U)strcpy(text,"debug");
        if (field==2U)strcpy(text,"bad token");
    }
    umi_diagnostic_pipeline_destroy(pipeline);
    return 0;
}
