/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/headless/view_model_headless.c
 *
 * PURPOSE:
 *   Render registered toolkit-neutral view presentations into deterministic
 *   text for tests, CLI inspection and non-graphical Framework consumers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This uses the same UmiUiViewPresentation that GTK4 renders. It is therefore a
 * useful architecture test: a Studio view is data first, not a hidden GTK widget.
 */

#include "headless_internal.h"

#include "umicom/ui/view_presentation.h"

UmiStatus umi_headless_render_pane_view(UmiUiHeadlessAdapter *adapter,
                                        UmiUiWorkbench *workbench,
                                        const UmiUiPaneSnapshot *pane)
{
    UmiUiViewPresentation presentation;
    UmiStatus status;
    size_t index;

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
    if (status != UMI_STATUS_OK) return status;

    status = umi_headless_append(adapter,
                                 "[view:%s]\ntype=%s\n",
                                 pane->pane_id,
                                 presentation.view.view_type);

    for (index = 0U;
         status == UMI_STATUS_OK && index < presentation.property_count;
         ++index) {
        char value[UMI_UI_VALUE_STRING_CAPACITY + 64U];
        const UmiUiPropertySnapshot *property =
            &presentation.properties[index];

        status = umi_ui_view_presentation_value_text(
            &property->value, value, sizeof(value));
        if (status == UMI_STATUS_OK) {
            status = umi_headless_append(adapter, "%s=%s\n",
                                         property->key, value);
        }
    }
    return status;
}
