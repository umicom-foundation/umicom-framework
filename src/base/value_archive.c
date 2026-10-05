/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/base/value_archive.c
 * PURPOSE: Inspect bounded state envelopes before typed decoding or persistence.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/base/value_archive.h"
#include "value_archive_internal.h"

UmiStatus umi_value_archive_inspect(const void *bytes, size_t byte_count,
    UmiValueArchiveInfo *out_info)
{
    if (bytes == NULL || out_info == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (CHAR_BIT != 8) return UMI_STATUS_UNAVAILABLE;
    if (byte_count > UMI_VALUE_ARCHIVE_BYTE_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (byte_count < UMI_VALUE_ARCHIVE_HEADER_SIZE) return UMI_STATUS_PARSE_ERROR;
    const unsigned char *input = (const unsigned char *)bytes;
    if (memcmp(input, "UMIVALUE", 8U) != 0 || UmiArchiveLoadInteger(input + 28U, 4U) != 0U)
        return UMI_STATUS_PARSE_ERROR;
    uint64_t payload = UmiArchiveLoadInteger(input + 16U, 8U);
    if (payload != (uint64_t)(byte_count - UMI_VALUE_ARCHIVE_HEADER_SIZE))
        return UMI_STATUS_PARSE_ERROR;
    if (UmiArchiveLoadInteger(input + 24U, 4U) != UmiArchiveChecksum(input, byte_count))
        return UMI_STATUS_PARSE_ERROR;
    UmiValueArchiveInfo info = {UmiArchiveLoadInteger(input + 8U, 8U), (size_t)payload};
    *out_info = info;
    return UMI_STATUS_OK;
}
