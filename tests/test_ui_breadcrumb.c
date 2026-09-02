/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ui_breadcrumb.c
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
#include <string.h>

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiUiBreadcrumbModel *model = NULL;
    UmiUiBreadcrumbSnapshot snapshot;

    assert(umi_ui_breadcrumb_model_create(&model) == UMI_STATUS_OK);
    assert(umi_ui_breadcrumb_set_path(
        model,
        "C:\\Dev\\umicom\\umicom-studio-ide\\applications\\studio") == UMI_STATUS_OK);
    assert(umi_ui_breadcrumb_snapshot(model, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.count >= 5U);
    assert(snapshot.segments[snapshot.count - 1U].current == 1);
    assert(strcmp(snapshot.segments[snapshot.count - 1U].label, "studio") == 0);

    umi_ui_breadcrumb_model_destroy(model);
    return 0;
}
