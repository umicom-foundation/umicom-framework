/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/data_session.c
 *
 * PURPOSE:
 *   Describe a caller Data Server session with routing/transaction context and revision evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/data_session.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialisation centralises bounded text handling and defaults. */
UmiStatus umi_data_data_session_init(UmiDataSession *item, const char *session_id, const char *principal_id, UmiDataConsistency consistency, uint64_t started_at) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->session_id,sizeof(item->session_id),session_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->principal_id,sizeof(item->principal_id),principal_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->consistency=consistency;item->started_at=started_at;item->last_activity=started_at;item->transaction_open=false;
    return umi_data_data_session_validate(item);
}

/* Validation prevents malformed metadata from leaking into later query/migration stages. */
UmiStatus umi_data_data_session_validate(const UmiDataSession *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->session_id, '\0', sizeof(item->session_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->principal_id, '\0', sizeof(item->principal_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (!(item->session_id[0] != '\0' && item->principal_id[0] != '\0' && item->consistency >= UMI_DATA_CONSISTENCY_EVENTUAL && item->consistency <= UMI_DATA_CONSISTENCY_STRONG)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDataSessionArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xb3f517cb8c0d3768);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataSession *)0)->session_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataSession *)0)->principal_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDataSessionArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDataSession *)0)->session_id) - 1U +
        8U + sizeof(((UmiDataSession *)0)->principal_id) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDataSessionArchiveWrite(UmiArchiveWriter *writer, const UmiDataSession *value)
{
    UmiArchiveWriteText(writer, value->session_id, sizeof(value->session_id));
    UmiArchiveWriteText(writer, value->principal_id, sizeof(value->principal_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->consistency);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->started_at);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->last_activity);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->transaction_open);
}
static void UmiDataSessionArchiveRead(UmiArchiveReader *reader, UmiDataSession *value)
{
    UmiArchiveReadText(reader, value->session_id, sizeof(value->session_id));
    UmiArchiveReadText(reader, value->principal_id, sizeof(value->principal_id));
    value->consistency = (UmiDataConsistency)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->started_at = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->last_activity = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->transaction_open = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDataSessionArchiveValidate(const UmiDataSession *value)
{
    return umi_data_data_session_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_data_data_session_archive_encode, umi_data_data_session_archive_decode,
    UmiDataSession, UmiDataSessionArchiveSchema, UmiDataSessionArchiveBound, UmiDataSessionArchiveWrite, UmiDataSessionArchiveRead, UmiDataSessionArchiveValidate)
