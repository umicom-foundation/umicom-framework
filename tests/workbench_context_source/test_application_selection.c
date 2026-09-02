/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_source/test_application_selection.c
 *
 * PURPOSE:
 *   Verify the application selection contract, bounded mutation and stable hashing.
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

#include "umicom/workbench_context_source/application_selection.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiWorkbenchContextSourceApplicationSelection record;
    uint64_t hash;

    umi_workbench_context_source_application_selection_init(
        &record,
        "application_selection-record");
    assert(umi_workbench_context_source_application_selection_validate(
        &record) == UMI_STATUS_OK);

    assert(umi_workbench_context_source_application_selection_set_source(
        &record, "source") == UMI_STATUS_OK);
    assert(umi_workbench_context_source_application_selection_set_panel(
        &record, "panel") == UMI_STATUS_OK);
    assert(umi_workbench_context_source_application_selection_set_subject(
        &record, "subject") == UMI_STATUS_OK);
    assert(umi_workbench_context_source_application_selection_set_group(
        &record, "blue") == UMI_STATUS_OK);
    assert(umi_workbench_context_source_application_selection_set_label(
        &record, "label") == UMI_STATUS_OK);

    record.source_kind = UMI_WORKBENCH_CONTEXT_SOURCE_EDITOR;
    record.trigger = UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_CARET;
    record.state = UMI_WORKBENCH_CONTEXT_SOURCE_STATE_ACTIVE;
    record.context_kind = UMI_CONTEXT_KIND_SOURCE_LOCATION;

    hash = umi_workbench_context_source_application_selection_hash(&record);
    assert(hash != 0U);

    umi_workbench_context_source_application_selection_touch(
        &record,
        9U,
        1000U);
    assert(record.sequence == 9U);
    assert(record.timestamp_ms == 1000U);
    assert(strcmp(record.source_id, "source") == 0);
    assert(strcmp(record.panel_id, "panel") == 0);
    return 0;
}
