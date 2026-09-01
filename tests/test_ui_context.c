/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ui_context.c
 *
 * PURPOSE:
 *   Exercise one Batch 23 unified-workbench contract through its public C API.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * These tests are intentionally small. They document expected behaviour without starting a graphical application.
 */

#include "umicom/umicom.h"
#include <assert.h>

int main(void)
{
    UmiUiContextStore *context = NULL;

    assert(umi_ui_context_store_create(&context) == UMI_STATUS_OK);
    assert(umi_ui_context_set_boolean(context, "studio.workspace.open", 1) == UMI_STATUS_OK);
    assert(umi_ui_context_set_string(context, "studio.mode", "develop") == UMI_STATUS_OK);
    assert(umi_ui_context_set_integer(context, "studio.selection.count", 2) == UMI_STATUS_OK);

    assert(umi_ui_context_evaluate(context, "studio.workspace.open"));
    assert(umi_ui_context_evaluate(context, "studio.mode=develop"));
    assert(umi_ui_context_evaluate(context,
        "studio.workspace.open && studio.selection.count!=0"));
    assert(!umi_ui_context_evaluate(context, "!studio.workspace.open"));
    assert(!umi_ui_context_evaluate(context, "studio.mode=test"));

    umi_ui_context_store_destroy(context);
    return 0;
}
