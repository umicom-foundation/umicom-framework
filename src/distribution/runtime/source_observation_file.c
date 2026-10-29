/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/source_observation_file.c
 *
 * PURPOSE:
 *   Retain source baseline files exclusively in an explicit owner-selected location.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/source_observation_archive.h"
#include "umicom/platform/input_file.h"
#include "umicom/platform/output_file.h"
#include <string.h>

UmiStatus UmiReleaseSourceObservationSave(const UmiReleaseSourceObservation *observation,
    const char *absolutePath, UmiSourceObservationReceipt *outReceipt)
{
    if (observation == NULL || absolutePath == NULL || outReceipt == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiOutputFileValidatePath(absolutePath);
    char *bytes = NULL;
    UmiSourceObservationReceipt receipt = {0};
    if (status == UMI_STATUS_OK)
        status = UmiReleaseSourceObservationEncode(observation, &bytes,
            &receipt.bytes, receipt.archiveSha256);
    if (status == UMI_STATUS_OK)
        memcpy(receipt.sourceSha256, UmiReleaseSourceObservationFingerprint(observation),
            sizeof(receipt.sourceSha256));
    UmiOutputFile *file = NULL;
    if (status == UMI_STATUS_OK) status = UmiOutputFileCreate(absolutePath, &file);
    if (status == UMI_STATUS_OK) status = UmiOutputFileWrite(file, bytes, receipt.bytes);
    if (file != NULL) {
        UmiStatus closed = UmiOutputFileClose(file);
        if (status == UMI_STATUS_OK) status = closed;
    }
    UmiOutputFileDestroy(file);
    UmiReleaseSourceObservationFreeEncoded(bytes);
    if (status == UMI_STATUS_OK) *outReceipt = receipt;
    return status;
}
UmiStatus UmiReleaseSourceObservationLoad(const UmiReleaseInventory *inventory,
    const char *absolutePath, const char *expectedSha256,
    UmiReleaseSourceObservation **outObservation)
{
    if (inventory == NULL || absolutePath == NULL || expectedSha256 == NULL ||
        outObservation == NULL || *outObservation != NULL) return UMI_STATUS_INVALID_ARGUMENT;
    unsigned char *bytes = NULL;
    size_t length = 0U;
    UmiStatus status = UmiInputFileRead(absolutePath, UMI_RELEASE_SOURCE_ARCHIVE_LIMIT - 1U, &bytes, &length);
    if (status == UMI_STATUS_OK)
        status = UmiReleaseSourceObservationDecode(inventory, (const char *)bytes,
            length, expectedSha256, outObservation);
    UmiInputFileFree(bytes);
    return status;
}
