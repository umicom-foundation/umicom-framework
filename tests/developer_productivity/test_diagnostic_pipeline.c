/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_productivity/test_diagnostic_pipeline.c
 *
 * PURPOSE:
 *   Verify multiline compiler output becomes Problems.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/developer_productivity/diagnostic_pipeline.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDeveloperDiagnosticPipeline *pipeline = NULL;
    size_t added = 0U;

    assert(umi_developer_diagnostic_pipeline_create(&pipeline) ==
           UMI_STATUS_OK);
    assert(umi_developer_diagnostic_pipeline_ingest_text(
        pipeline,
        "src/a.c:10:3: error: bad thing\n"
        "ninja: error: subcommand failed\n",
        &added) == UMI_STATUS_OK);
    assert(added >= 2U);

    umi_developer_diagnostic_pipeline_destroy(pipeline);
    return 0;
}
