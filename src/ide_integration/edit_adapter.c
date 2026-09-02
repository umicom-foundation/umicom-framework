/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/edit_adapter.c
 *
 * PURPOSE:
 *   Validate the editor mutation adapter required for inline completion.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/edit_adapter.h"

/*
 * Check that ide editor edit adapter satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_ide_editor_edit_adapter_validate(
    const UmiIdeEditorEditAdapter *adapter)
{
    return adapter != NULL &&
           adapter->get_revision != NULL &&
           adapter->insert_text != NULL
        ? UMI_STATUS_OK
        : UMI_STATUS_INVALID_ARGUMENT;
}
