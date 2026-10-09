/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/widget_lookup_gtk4.c
 * PURPOSE: Expose logical widget lookup to full and standalone GTK components.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/automation.h"
#include "widget_lookup_internal.h"
/* Lookup inspects ownership but never activates, reveals or focuses a control. */
/* A fixture may inspect an unpresented panel without activating it. Borrow
 * the unique result from the caller-owned tree; traversal itself retains a
 * reference only long enough to detect ambiguity safely. */
void *umi_gtk4_automation_find_tagged_widget(void *native_root, const char *automation_id)
{
    if (native_root == NULL || !GTK_IS_WIDGET(native_root) ||
        automation_id == NULL || automation_id[0] == '\0') return NULL;
    AutomationSearch search = {0};
    automation_find_widgets(GTK_WIDGET(native_root), automation_id, NULL, 0U, &search);
    GtkWidget *result = search.status == UMI_STATUS_OK && search.matches == 1U
        ? search.found : NULL;
    g_clear_object(&search.found);
    return result;
}

/* Compatibility lookup shares the same bounds and logical-child traversal as
 * tagged controls. It cannot broaden the driver to unrelated top-level windows. */
void *umi_gtk4_automation_find_named_widget(void *native_root, const char *widget_name)
{
    if (native_root == NULL || !GTK_IS_WIDGET(native_root) ||
        widget_name == NULL || widget_name[0] == '\0') return NULL;
    AutomationSearch search = {0};
    search.by_widget_name = true;
    automation_find_widgets(GTK_WIDGET(native_root), widget_name, NULL, 0U, &search);
    GtkWidget *result = search.status == UMI_STATUS_OK && search.matches == 1U
        ? search.found : NULL;
    g_clear_object(&search.found);
    return result;
}

