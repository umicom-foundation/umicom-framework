/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_source/test_sample.c
 *
 * PURPOSE:
 *   Verify real interaction sample identity, location, metadata and hashing.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/workbench_context_source/sample.h"

int main(void)
{
    UmiWorkbenchContextSourceSample sample;
    umi_workbench_context_source_sample_init(
        &sample,
        UMI_WORKBENCH_CONTEXT_SOURCE_EDITOR,
        UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_CARET,
        UMI_CONTEXT_KIND_SOURCE_LOCATION,
        "sample");
    assert(umi_workbench_context_source_sample_set_identity(
        &sample,
        "studio.editor.location",
        "org.umicom.studio",
        "studio.editor",
        "workspace") == UMI_STATUS_OK);
    assert(umi_workbench_context_source_sample_set_subject(
        &sample, "main.c", "main") == UMI_STATUS_OK);
    assert(umi_workbench_context_source_sample_set_location(
        &sample, "C:/dev/main.c", "main",
        12U, 4U, 3U) == UMI_STATUS_OK);
    assert(umi_workbench_context_source_sample_add_metadata(
        &sample, "language-id", "c") == UMI_STATUS_OK);
    assert(umi_workbench_context_source_sample_validate(
        &sample) == UMI_STATUS_OK);
    assert(umi_workbench_context_source_sample_refresh_hash(
        &sample) != 0U);

    return 0;
}
