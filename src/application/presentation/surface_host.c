/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/surface_host.c
 *
 * PURPOSE:
 *   Validate frontend host contracts before live application surfaces begin.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/presentation/surface_host.h"

/*
 * Check that application presentation surface host satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_application_presentation_surface_host_validate(
    const UmiApplicationPresentationSurfaceHost *host)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (host == NULL || host->operations == NULL ||
        host->operations->struct_size != sizeof(*host->operations) ||
        host->operations->begin == NULL || host->operations->mount == NULL ||
        host->operations->update == NULL || host->operations->focus == NULL ||
        host->operations->unmount == NULL || host->operations->end == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}
