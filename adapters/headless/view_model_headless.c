/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/headless/view_model_headless.c
 *
 * PURPOSE:
 *   Render registered toolkit-neutral view presentations into deterministic
 *   text for tests, CLI inspection and non-graphical Framework consumers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This uses the same UmiUiViewPresentation that GTK4 renders. It is therefore a
 * useful architecture test: a Studio view is data first, not a hidden GTK widget.
 */

#include "headless_internal.h"

#include "umicom/ui/view_presentation.h"

/*
 * Provide the headless render pane view operation used by this module and its client
 * applications.
 */
UmiStatus umi_headless_render_pane_view(UmiUiHeadlessAdapter *adapter,
                                        UmiUiWorkbench *workbench,
                                        const UmiUiPaneSnapshot *pane)
{
    UmiUiViewPresentation presentation;
    UmiStatus status;
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL || workbench == NULL || pane == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ui_view_presentation_build(
        umi_ui_workbench_view_factories(workbench),
        pane->view_type,
        pane->pane_id,
        &presentation);

    /* A missing factory is legal during incremental migration. */
    if (status == UMI_STATUS_NOT_FOUND) return UMI_STATUS_OK;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_headless_append(adapter,
                                 "[view:%s]\ntype=%s\n",
                                 pane->pane_id,
                                 presentation.view.view_type);

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         status == UMI_STATUS_OK && index < presentation.property_count;
         ++index) {
        char value[UMI_UI_VALUE_STRING_CAPACITY + 64U];
        const UmiUiPropertySnapshot *property =
            &presentation.properties[index];

        status = umi_ui_view_presentation_value_text(
            &property->value, value, sizeof(value));
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            status = umi_headless_append(adapter, "%s=%s\n",
                                         property->key, value);
        }
    }
    return status;
}
