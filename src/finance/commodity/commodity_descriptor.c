/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/commodity/commodity_descriptor.c
 *
 * PURPOSE:
 *   Implement reference-data metadata for a physical or financially settled commodity.
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

#include "umicom/finance/commodity/commodity_descriptor.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_commodity_commodity_descriptor_init(UmiCommodityDescriptor *value, const char *id, const char *name, const char *code, UmiCommodityKind kind, const UmiCurrency *currency, bool physical_delivery)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || currency == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    memset(value, 0, sizeof *value);
    status = umi_commodity_copy_text(value->id.value, sizeof value->id.value, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->name, sizeof value->name, name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->code, sizeof value->code, code);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->kind = kind;
    value->settlement_currency = *currency;
    value->physical_delivery = physical_delivery;
    value->active = true;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_commodity_commodity_descriptor_valid(const UmiCommodityDescriptor *value)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id.value, '\0', sizeof(value->id.value)) == NULL) return 0;
    if (memchr(value->name, '\0', sizeof(value->name)) == NULL) return 0;
    if (memchr(value->code, '\0', sizeof(value->code)) == NULL) return 0;
    if (memchr(value->settlement_currency.code, '\0', sizeof(value->settlement_currency.code)) == NULL) return 0;

    return value != NULL && (umi_commodity_text_valid(value->id.value) && umi_commodity_text_valid(value->code) && value->kind != UMI_COMMODITY_KIND_UNKNOWN && value->active);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiCommodityDescriptorArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xe53ae3515564a012);
    schema = (schema ^ (uint64_t)sizeof(((UmiCommodityDescriptor *)0)->id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiCommodityDescriptor *)0)->name)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiCommodityDescriptor *)0)->code)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiCommodityDescriptor *)0)->settlement_currency.code)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiCommodityDescriptorArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiCommodityDescriptor *)0)->id.value) - 1U +
        8U + sizeof(((UmiCommodityDescriptor *)0)->name) - 1U +
        8U + sizeof(((UmiCommodityDescriptor *)0)->code) - 1U +
        8U +
        8U + sizeof(((UmiCommodityDescriptor *)0)->settlement_currency.code) - 1U +
        8U +
        8U;
}
static void UmiCommodityDescriptorArchiveWrite(UmiArchiveWriter *writer, const UmiCommodityDescriptor *value)
{
    UmiArchiveWriteText(writer, value->id.value, sizeof(value->id.value));
    UmiArchiveWriteText(writer, value->name, sizeof(value->name));
    UmiArchiveWriteText(writer, value->code, sizeof(value->code));
    UmiArchiveWriteSigned(writer, (int64_t)value->kind);
    UmiArchiveWriteText(writer, value->settlement_currency.code, sizeof(value->settlement_currency.code));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->physical_delivery);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->active);
}
static void UmiCommodityDescriptorArchiveRead(UmiArchiveReader *reader, UmiCommodityDescriptor *value)
{
    UmiArchiveReadText(reader, value->id.value, sizeof(value->id.value));
    UmiArchiveReadText(reader, value->name, sizeof(value->name));
    UmiArchiveReadText(reader, value->code, sizeof(value->code));
    value->kind = (UmiCommodityKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    UmiArchiveReadText(reader, value->settlement_currency.code, sizeof(value->settlement_currency.code));
    value->physical_delivery = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->active = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiCommodityDescriptorArchiveValidate(const UmiCommodityDescriptor *value)
{
    return umi_commodity_commodity_descriptor_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_commodity_commodity_descriptor_archive_encode, umi_commodity_commodity_descriptor_archive_decode,
    UmiCommodityDescriptor, UmiCommodityDescriptorArchiveSchema, UmiCommodityDescriptorArchiveBound, UmiCommodityDescriptorArchiveWrite, UmiCommodityDescriptorArchiveRead, UmiCommodityDescriptorArchiveValidate)
