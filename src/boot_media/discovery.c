/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/boot_media/discovery.c
 *
 * PURPOSE:
 *   Publish a fresh read-only inventory report through one shared native-adapter boundary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"

/* Native adapters own observation and device policy. This boundary owns the
 * lifetime of public diagnostics, so an inventory refresh cannot inherit an
 * earlier transfer's verified flag, progress or write-started state. */
UmiStatus UmiBootMediaDiscover(UmiBootMediaInventory *out, UmiBootMediaReport *report)
{
    UmiBootMediaReport candidate = {0};
    UmiStatus status;
    if (out == NULL) {
        status = BmMessage(&candidate, UMI_STATUS_INVALID_ARGUMENT,
            "An output inventory is required. No device query was started.");
    } else {
        memset(out, 0, sizeof(*out));
        status = BmDiscover(out, &candidate);
        /* Preserve the adapter's bounded explanation and partial inventory,
         * including capacity refusal. No inventory result represents a transfer. */
        candidate.status = status;
        candidate.phase = UMI_BOOT_MEDIA_INSPECT;
        candidate.bytesDone = 0U;
        candidate.bytesTotal = 0U;
        candidate.outputCreated = 0;
        candidate.writeStarted = 0;
        candidate.verified = 0;
        candidate.detail[sizeof(candidate.detail) - 1U] = '\0';
        if (candidate.detail[0] == '\0')
            (void)BmMessage(&candidate, status, status == UMI_STATUS_OK
                ? "Read-only removable-device inventory completed."
                : "Read-only removable-device inventory failed.");
    }
    if (report != NULL) *report = candidate;
    return status;
}
