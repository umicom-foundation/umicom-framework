/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/headless/dialogs_headless.c
 *
 * PURPOSE:
 *   Render the number of pending frontend-neutral dialog requests without blocking.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "headless_internal.h"

/*
 * Provide the headless render dialogs operation used by this module and its client
 * applications.
 */
UmiStatus umi_headless_render_dialogs(UmiUiHeadlessAdapter *adapter,
                                      UmiUiWorkbench *workbench)
{
    return umi_headless_append(adapter, "dialogs-pending=%zu\n",
        umi_ui_dialog_pending_count(umi_ui_workbench_dialogs(workbench)));
}
