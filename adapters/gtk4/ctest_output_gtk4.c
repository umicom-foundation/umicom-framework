/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ctest_output_gtk4.c
 * PURPOSE: Translate test attempt evidence into the shared bounded output view.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/testing/ctest_output_gtk4.h"
#include "umicom/ui/gtk4/output_view.h"
#include <string.h>

GtkWidget *UmiCtestOutputGtk4Create(void)
{
    GtkWidget *panel =
        UmiOutputViewGtk4Create("tests.output", "Ready. Run tests to see their captured output here.");
    g_object_set_data(G_OBJECT(panel), "umicom-ctest-output-adapter", GINT_TO_POINTER(1));
    return panel;
}

UmiStatus UmiCtestOutputGtk4Update(GtkWidget *panel, const UmiCtestOutputSnapshot *snapshot)
{
    if (panel == NULL || !GTK_IS_BOX(panel) || snapshot == NULL ||
        g_object_get_data(G_OBJECT(panel), "umicom-ctest-output-adapter") == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (snapshot->tail.length >= sizeof(snapshot->bytes) || snapshot->bytes[snapshot->tail.length] != '\0' ||
        snapshot->tail.total_bytes < snapshot->tail.length ||
        memchr(snapshot->name, '\0', sizeof(snapshot->name)) == NULL ||
        memchr(snapshot->test_id, '\0', sizeof(snapshot->test_id)) == NULL ||
        (snapshot->invocation != 0U &&
         (snapshot->task_id == 0U || snapshot->revision == 0U || snapshot->attempt == 0U ||
          snapshot->name[0] == '\0' || snapshot->test_id[0] == '\0')) ||
        (snapshot->invocation == 0U &&
         (snapshot->revision != 0U || snapshot->tail.length != 0U || snapshot->attempt_complete)))
        return UMI_STATUS_INVALID_ARGUMENT;
    _Static_assert(UMI_CTEST_OUTPUT_CAPACITY <= UMI_OUTPUT_VIEW_CAPACITY,
                   "Output view must retain test tail");
    UmiOutputViewSnapshot *view = g_new0(UmiOutputViewSnapshot, 1);
    view->operation_id = snapshot->task_id;
    view->revision = snapshot->revision;
    view->length = snapshot->tail.length;
    view->total_bytes = snapshot->tail.total_bytes;
    view->truncated = snapshot->tail.truncated;
    view->counters_saturated = snapshot->tail.counters_saturated;
    memcpy(view->bytes, snapshot->bytes, snapshot->tail.length + 1U);
    if (snapshot->task_id != 0U)
    {
        if (snapshot->invocation == 0U)
        {
            (void)g_snprintf(view->context, sizeof(view->context), "No test attempt has started.");
            (void)g_snprintf(view->status, sizeof(view->status),
                             "Inspect the Test Explorer summary for queue or cancellation state.");
        }
        else
        {
            (void)g_snprintf(view->context, sizeof(view->context), "%s | attempt %u | invocation %zu",
                             snapshot->name, snapshot->attempt, snapshot->invocation);
            const char *outcome =
                !snapshot->attempt_complete                        ? "running; result pending"
                : snapshot->result_state == UMI_TEST_STATE_SKIPPED ? "skipped; no pass is implied"
                : snapshot->result_state == UMI_TEST_STATE_PASSED && snapshot->result_status == UMI_STATUS_OK
                    ? "passed"
                : snapshot->result_status == UMI_STATUS_OK ? "not passed; inspect results"
                                                           : umi_status_text(snapshot->result_status);
            (void)g_snprintf(view->status, sizeof(view->status), "%s%s", outcome,
                             snapshot->capture_status != UMI_STATUS_OK
                                 ? " | output capture failed; inspect completed results"
                                 : "");
        }
    }
    UmiStatus status = UmiOutputViewGtk4Update(panel, view);
    g_free(view);
    return status;
}
