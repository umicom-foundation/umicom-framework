/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/data_server_profile.c
 *
 * PURPOSE:
 *   Describe logical Data Server operating limits and consistency defaults for deployment profiles.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/data_server_profile.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialisation centralises bounded text handling and defaults. */
UmiStatus umi_data_data_server_profile_init(UmiDataServerProfile *item, const char *profile_id, size_t minimum_pool_size, size_t maximum_pool_size, uint64_t query_row_limit) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->profile_id,sizeof(item->profile_id),profile_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->minimum_pool_size=minimum_pool_size;item->maximum_pool_size=maximum_pool_size;item->query_row_limit=query_row_limit;item->default_consistency=UMI_DATA_CONSISTENCY_SESSION;item->migrations_enabled=true;
    return umi_data_data_server_profile_validate(item);
}

/* Validation prevents malformed metadata from leaking into later query/migration stages. */
UmiStatus umi_data_data_server_profile_validate(const UmiDataServerProfile *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->profile_id, '\0', sizeof(item->profile_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (!(item->profile_id[0] != '\0' && item->minimum_pool_size <= item->maximum_pool_size && item->maximum_pool_size <= UMI_DATA_ENTERPRISE_MAX_ITEMS && item->query_row_limit > 0U)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDataServerProfileArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x807959e4edcfd19c);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataServerProfile *)0)->profile_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDataServerProfileArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDataServerProfile *)0)->profile_id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDataServerProfileArchiveWrite(UmiArchiveWriter *writer, const UmiDataServerProfile *value)
{
    UmiArchiveWriteText(writer, value->profile_id, sizeof(value->profile_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->minimum_pool_size);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->maximum_pool_size);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->query_row_limit);
    UmiArchiveWriteSigned(writer, (int64_t)value->default_consistency);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->migrations_enabled);
}
static void UmiDataServerProfileArchiveRead(UmiArchiveReader *reader, UmiDataServerProfile *value)
{
    UmiArchiveReadText(reader, value->profile_id, sizeof(value->profile_id));
    value->minimum_pool_size = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->maximum_pool_size = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->query_row_limit = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->default_consistency = (UmiDataConsistency)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->migrations_enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDataServerProfileArchiveValidate(const UmiDataServerProfile *value)
{
    return umi_data_data_server_profile_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_data_data_server_profile_archive_encode, umi_data_data_server_profile_archive_decode,
    UmiDataServerProfile, UmiDataServerProfileArchiveSchema, UmiDataServerProfileArchiveBound, UmiDataServerProfileArchiveWrite, UmiDataServerProfileArchiveRead, UmiDataServerProfileArchiveValidate)
