#include <assert.h>
#include "umicom/diagnostic_ui/diagnostic_ui.h"
int main(void)
{
    UmiDiagnosticPipeline *pipeline = NULL;
    UmiUiViewModel *problems = NULL;
    UmiUiViewModel *output = NULL;
    UmiUiValue value;
    assert(umi_diagnostic_pipeline_create(NULL, &pipeline) == UMI_STATUS_OK);
    assert(umi_diagnostic_pipeline_ingest_line(pipeline, "build", "Build", "gcc",
        UMI_OUTPUT_STREAM_ERROR, "a.c:1:1: error: failure", 1U) == UMI_STATUS_OK);
    assert(umi_diagnostic_problems_view_create("problems", pipeline, &problems) == UMI_STATUS_OK);
    assert(umi_diagnostic_output_view_create("output", pipeline, NULL, &output) == UMI_STATUS_OK);
    assert(umi_ui_view_model_get_property(problems, "problem.count", &value) == UMI_STATUS_OK);
    assert(value.integer_value == 1);
    assert(umi_ui_view_model_get_property(output, "umicom.view-kind", &value) == UMI_STATUS_OK);
    umi_ui_view_model_destroy(output);
    umi_ui_view_model_destroy(problems);
    umi_diagnostic_pipeline_destroy(pipeline);
    return 0;
}
