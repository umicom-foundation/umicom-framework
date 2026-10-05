/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/digital_asset/key_reference.c
 *
 * PURPOSE:
 *   Reference signing-key material by provider identifier without storing secret key bytes.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/digital_asset/key_reference.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_digital_asset_key_reference_init(UmiDigitalKeyReference *value, const char *id, const char *provider_reference, bool hardware_backed)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_digital_asset_copy_text(value->id.value, sizeof value->id.value, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->provider_reference, sizeof value->provider_reference, provider_reference);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->hardware_backed = hardware_backed;
    value->active = true;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_digital_asset_key_reference_valid(const UmiDigitalKeyReference *value)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id.value, '\0', sizeof(value->id.value)) == NULL) return 0;
    if (memchr(value->provider_reference, '\0', sizeof(value->provider_reference)) == NULL) return 0;

    return value != NULL && (umi_digital_asset_text_valid(value->id.value) && umi_digital_asset_text_valid(value->provider_reference) && value->active);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDigitalKeyReferenceArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x5f8a77267f923acf);
    schema = (schema ^ (uint64_t)sizeof(((UmiDigitalKeyReference *)0)->id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDigitalKeyReference *)0)->provider_reference)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDigitalKeyReferenceArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDigitalKeyReference *)0)->id.value) - 1U +
        8U + sizeof(((UmiDigitalKeyReference *)0)->provider_reference) - 1U +
        8U +
        8U;
}
static void UmiDigitalKeyReferenceArchiveWrite(UmiArchiveWriter *writer, const UmiDigitalKeyReference *value)
{
    UmiArchiveWriteText(writer, value->id.value, sizeof(value->id.value));
    UmiArchiveWriteText(writer, value->provider_reference, sizeof(value->provider_reference));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->hardware_backed);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->active);
}
static void UmiDigitalKeyReferenceArchiveRead(UmiArchiveReader *reader, UmiDigitalKeyReference *value)
{
    UmiArchiveReadText(reader, value->id.value, sizeof(value->id.value));
    UmiArchiveReadText(reader, value->provider_reference, sizeof(value->provider_reference));
    value->hardware_backed = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->active = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDigitalKeyReferenceArchiveValidate(const UmiDigitalKeyReference *value)
{
    return umi_digital_asset_key_reference_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_digital_asset_key_reference_archive_encode, umi_digital_asset_key_reference_archive_decode,
    UmiDigitalKeyReference, UmiDigitalKeyReferenceArchiveSchema, UmiDigitalKeyReferenceArchiveBound, UmiDigitalKeyReferenceArchiveWrite, UmiDigitalKeyReferenceArchiveRead, UmiDigitalKeyReferenceArchiveValidate)
