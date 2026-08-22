/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_source/test_definition.c
 *
 * PURPOSE:
 *   Verify configured sources accept only matching identity and typed context samples.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/workbench_context_source/definition.h"

int main(void)
{
    UmiWorkbenchContextSourceDefinition definition;
    UmiWorkbenchContextSourceSample sample;
    umi_workbench_context_source_definition_init(
        &definition, "studio.editor.location");
    assert(umi_workbench_context_source_definition_set_identity(
        &definition, "org.umicom.studio",
        "studio.editor", "Editor") == UMI_STATUS_OK);
    definition.source_kind = UMI_WORKBENCH_CONTEXT_SOURCE_EDITOR;
    definition.trigger = UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_CARET;
    definition.context_kind = UMI_CONTEXT_KIND_SOURCE_LOCATION;
    definition.accepted_kinds_mask =
        UINT64_C(1) << ((unsigned)UMI_CONTEXT_KIND_SOURCE_LOCATION - 1U);
    assert(umi_workbench_context_source_definition_validate(
        &definition) == UMI_STATUS_OK);

    umi_workbench_context_source_sample_init(
        &sample,
        UMI_WORKBENCH_CONTEXT_SOURCE_EDITOR,
        UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_CARET,
        UMI_CONTEXT_KIND_SOURCE_LOCATION,
        "sample");
    assert(umi_workbench_context_source_sample_set_identity(
        &sample, "studio.editor.location",
        "org.umicom.studio", "studio.editor",
        "workspace") == UMI_STATUS_OK);
    assert(umi_workbench_context_source_definition_accepts(
        &definition, &sample));

    return 0;
}
