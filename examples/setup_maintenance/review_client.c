/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * File: examples/setup_maintenance/review_client.c
 * Read-only example: explain a candidate update without applying it.
 * The caller owns the strings and output stream; Framework owns the plan.
 *---------------------------------------------------------------------------*/
#include "review_client.h"
#include <inttypes.h>
UmiStatus UmiMaintenanceReviewExample(const char *installation,
    const char *release, FILE *output)
{
    if (installation == NULL || release == NULL || output == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiSetupBundle *installed = NULL;
    UmiSetupMaintenancePlan *plan = NULL;
    UmiSetupMaintenanceReport report = {0};
    UmiStatus status = UmiSetupInstalledBundleOpen(installation, &installed, &report.io);
    if (status != UMI_STATUS_OK) goto finish;

    /* Bind the operation to the application list we have just read. A changed
     * receipt causes a new review rather than changing the meaning of a mask. */
    UmiSetupMaintenanceConfig config = {
        .installationRoot = installation,
        .releaseRoot = release,
        .action = UMI_SETUP_MAINTENANCE_UPDATE,
        .removeMask = 0U,
        .expectedReceipt = UmiSetupMaintenanceReceiptIdentity(installed)
    };
    status = UmiSetupMaintenancePlanCreate(&config, &plan, NULL, NULL, &report);
    if (status != UMI_STATUS_OK) goto finish;
    const UmiSetupMaintenanceSummary *summary = UmiSetupMaintenancePlanSummary(plan);
    if (fprintf(output, "Plan: %s\nReplacement bytes: %" PRIu64 "\n",
            summary->fingerprint, summary->stagingBytes) < 0) {
        status = UMI_STATUS_IO_ERROR; goto finish;
    }
    for (size_t i = 0U; i < summary->checkedFiles; ++i) {
        const UmiSetupMaintenanceRow *row = UmiSetupMaintenancePlanRow(plan, i);
        if (fprintf(output, "%s: %s\n", UmiSetupMaintenanceChangeText(row->change),
                row->relative) < 0) { status = UMI_STATUS_IO_ERROR; goto finish; }
    }
    if (fputs("Review only. Nothing was installed, removed or started.\n", output) == EOF)
        status = UMI_STATUS_IO_ERROR;
finish:
    /* Destroy the owned objects once on every exit path. Never free a borrowed
     * summary/row pointer: its storage belongs to the plan until Destroy. */
    UmiSetupMaintenancePlanDestroy(plan);
    UmiSetupBundleDestroy(installed);
    return status;
}
