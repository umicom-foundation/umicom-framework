/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/os_image/firmware.c
 *
 * PURPOSE:
 *   Verify selected firmware before any output is created or guest process is started.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include "umicom/os_image/boot_firmware.h"

/* The boot service owns this bounded copy. Reading and hashing the same bytes
 * avoids accepting one file identity and later copying unrelated file content. */
UmiStatus OiFirmwareRead(const UmiOsImageFirmware *pin,
    unsigned char **data, size_t *size)
{
    if (!data || !size) return UMI_STATUS_INVALID_ARGUMENT;
    *data = NULL;
    *size = 0;
    if (!pin || !OiHash(pin->sha256, 64) || !pin->bytes ||
        pin->bytes > UMI_OS_IMAGE_MAX_FILE ||
        OiAbsolute(pin->path, 0) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;

    unsigned char *bytes = NULL;
    size_t length = 0;
    UmiStatus status = OiRead(pin->path, UMI_OS_IMAGE_MAX_FILE, &bytes, &length);
    char hash[65];
    if (status == UMI_STATUS_OK && (uint64_t)length != pin->bytes)
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK)
        status = UmiNativeSha256Buffer(bytes, length, hash);
    if (status == UMI_STATUS_OK && strcmp(hash, pin->sha256))
        status = UMI_STATUS_INVALID_STATE;
    if (status != UMI_STATUS_OK) {
        free(bytes);
        return status;
    }
    *data = bytes;
    *size = length;
    return UMI_STATUS_OK;
}

UmiStatus UmiOsImageCheckFirmware(const UmiOsImageFirmware *pin,
    UmiOsImageReport *report)
{
    OiInit(report);
    unsigned char *data = NULL;
    size_t size = 0;
    UmiStatus status = OiFirmwareRead(pin, &data, &size);
    free(data);
    if (status == UMI_STATUS_OK && report) {
        report->completed = 1;
        report->bytes = (uint64_t)size;
        report->files = 1;
    }
    return OiReport(report, status, status == UMI_STATUS_OK ?
        "Firmware bytes match the supplied pin. No guest was executed." :
        "Firmware was not accepted. Check its explicit path, digest and byte count.");
}
