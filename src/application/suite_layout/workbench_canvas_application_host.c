/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/suite_layout/workbench_canvas_application_host.c
 *
 * PURPOSE:
 *   Connect the application-suite experience catalogue to the reusable
 *   Workbench Canvas coordinator.  The canvas data structure itself remains
 *   in the UI library; this small bridge lives in the application library so
 *   the lower-level UI archive does not depend on the application archive.
 *
 *   Keeping this bridge at the suite-layout boundary avoids a static-library
 *   dependency cycle while preserving one public operation for Studio, Trader
 *   and the other thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workbench_canvas.h"

#include "umicom/application/suite_layout/customisation.h"

/* Load the complete product experience before exposing its native host. */
UmiStatus umi_ui_workbench_canvas_add_application_host(
    UmiUiWorkbenchCanvas *canvas,
    const char *host_id,
    const char *application_id,
    const char *monitor_id,
    UmiUiWorkspaceCustomisation *customisation)
{
    UmiStatus status;

    /* Validate host inputs before loading the experience so a bad native
     * window identity cannot replace an already usable customisation model. */
    if (canvas == NULL || host_id == NULL || application_id == NULL ||
        monitor_id == NULL || customisation == NULL || host_id[0] == '\0' ||
        application_id[0] == '\0' || monitor_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Reject duplicate hosts and a full canvas before changing application
     * state; these are deterministic failures known without catalogue work. */
    if (umi_ui_workbench_canvas_host_const(canvas, host_id) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    if (canvas->host_count >= UMI_UI_WORKBENCH_CANVAS_MAX_HOSTS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* The suite loader validates the canonical experience, registers every
     * panel, creates every named layout and activates its default layout. */
    status = umi_application_suite_customisation_load_application(
        customisation, application_id);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    /* Register the host only after the complete product model is available so
     * frontends never observe a host whose catalogue is only partly loaded. */
    return umi_ui_workbench_canvas_add_host(
        canvas, host_id, application_id, monitor_id, customisation);
}
