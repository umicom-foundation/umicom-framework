/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/inspection/index_probe.c
 *
 * PURPOSE:
 *   Implement populate the existing index maintenance model from porcelain-v2 output.
 *
 * ARCHITECTURE:
 *   Extend the existing Framework-owned repository control and maintenance
 *   capabilities. This module does not duplicate VCS, source-control, lock,
 *   maintenance or doctor state already present in Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/repository/index_probe.h"

#include "umicom/repository/status_probe.h"

/* Project the requested maintenance model from one shared porcelain status object. */
UmiStatus umi_repository_index_probe_read(
    const UmiRepositoryInspectionContext *context,
    UmiRepositoryIndexStatus *out_value)
{
    UmiRepositoryPorcelainStatus parsed;
    UmiStatus status;

    /* Both the operation context and caller-owned output are mandatory. */
    if (context == NULL || out_value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Delegate Git execution and parsing to the shared status probe. */
    status = umi_repository_status_probe_read(context, &parsed);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    /* Return only the established maintenance model requested by the caller. */
    *out_value = parsed.index;
    return UMI_STATUS_OK;
}
