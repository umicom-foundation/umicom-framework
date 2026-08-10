/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/menu_gtk4.c
 *
 * PURPOSE:
 *   Render toolkit-neutral menu contributions as a compact GTK4 menu-summary row
 *   without owning product command behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "gtk4_internal.h"

UmiStatus umi_gtk4_refresh_menu(UmiGtk4Adapter *adapter, UmiUiWorkbench *workbench)
{
    UmiUiMenuModel *model;
    size_t index;
    if (adapter == NULL || workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_gtk4_clear_box(adapter->menu_bar);
    model = umi_ui_workbench_menus(workbench);
    for (index = 0U; index < umi_ui_menu_model_count(model); ++index) {
        UmiUiMenuSnapshot item;
        if (umi_ui_menu_model_at(model, index, &item) == UMI_STATUS_OK && !item.separator) {
            GtkWidget *label = gtk_label_new(item.label_override[0] != '\0' ? item.label_override : item.action_id);
            gtk_widget_add_css_class(label, "menu");
            gtk_box_append(GTK_BOX(adapter->menu_bar), label);
        }
    }
    return UMI_STATUS_OK;
}
