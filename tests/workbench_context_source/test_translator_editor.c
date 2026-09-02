/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_source/test_translator_editor.c
 *
 * PURPOSE:
 *   Verify editor samples translate into canonical source-location interaction events.
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

#include "umicom/workbench_context_source/translator.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiWorkbenchContextSourceDefinition definition;
    UmiWorkbenchContextSourceSample sample;
    UmiWorkbenchContextEvent event;
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

    umi_workbench_context_source_sample_init(
        &sample, UMI_WORKBENCH_CONTEXT_SOURCE_EDITOR,
        UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_CARET,
        UMI_CONTEXT_KIND_SOURCE_LOCATION, "editor-event");
    assert(umi_workbench_context_source_sample_set_identity(
        &sample, "studio.editor.location",
        "org.umicom.studio", "studio.editor",
        "workspace") == UMI_STATUS_OK);
    assert(umi_workbench_context_source_sample_set_location(
        &sample, "C:/dev/main.c", "main",
        20U, 8U, 0U) == UMI_STATUS_OK);
    assert(umi_workbench_context_source_translate(
        &definition, &sample, &event) == UMI_STATUS_OK);
    assert(event.kind == UMI_WORKBENCH_CONTEXT_EVENT_EDITOR_CARET);
    assert(event.context_kind == UMI_CONTEXT_KIND_SOURCE_LOCATION);
    assert(event.line == 20U);
    assert(event.column == 8U);

    return 0;
}
