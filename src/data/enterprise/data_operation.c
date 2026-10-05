/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/data_operation.c
 *
 * PURPOSE:
 *   Describe one reviewable Data Server operation for queueing, audit and cancellation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/data_operation.h"
#include "../../base/value_archive_internal.h"
#include <string.h>

/* Initialisation centralises bounded text handling and defaults. */
UmiStatus umi_data_data_operation_init(UmiDataOperation *item, const char *operation_id, const char *session_id, const char *operation_kind, uint64_t submitted_at, uint32_t priority) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->operation_id,sizeof(item->operation_id),operation_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->session_id,sizeof(item->session_id),session_id);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->operation_kind,sizeof(item->operation_kind),operation_kind);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;item->submitted_at=submitted_at;item->priority=priority;item->cancellable=true;
    return umi_data_data_operation_validate(item);
}

/* Validation prevents malformed metadata from leaking into later query/migration stages. */
UmiStatus umi_data_data_operation_validate(const UmiDataOperation *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->operation_id, '\0', sizeof(item->operation_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->session_id, '\0', sizeof(item->session_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(item->operation_kind, '\0', sizeof(item->operation_kind)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(item->operation_id[0] != '\0' && item->session_id[0] != '\0' && item->operation_kind[0] != '\0')) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDataOperationArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xd21ab5bdb9282ea8);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataOperation *)0)->operation_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataOperation *)0)->session_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDataOperation *)0)->operation_kind)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDataOperationArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDataOperation *)0)->operation_id) - 1U +
        8U + sizeof(((UmiDataOperation *)0)->session_id) - 1U +
        8U + sizeof(((UmiDataOperation *)0)->operation_kind) - 1U +
        8U +
        8U +
        8U;
}
static void UmiDataOperationArchiveWrite(UmiArchiveWriter *writer, const UmiDataOperation *value)
{
    UmiArchiveWriteText(writer, value->operation_id, sizeof(value->operation_id));
    UmiArchiveWriteText(writer, value->session_id, sizeof(value->session_id));
    UmiArchiveWriteText(writer, value->operation_kind, sizeof(value->operation_kind));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->submitted_at);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->priority);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->cancellable);
}
static void UmiDataOperationArchiveRead(UmiArchiveReader *reader, UmiDataOperation *value)
{
    UmiArchiveReadText(reader, value->operation_id, sizeof(value->operation_id));
    UmiArchiveReadText(reader, value->session_id, sizeof(value->session_id));
    UmiArchiveReadText(reader, value->operation_kind, sizeof(value->operation_kind));
    value->submitted_at = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->priority = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->cancellable = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDataOperationArchiveValidate(const UmiDataOperation *value)
{
    return umi_data_data_operation_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_data_data_operation_archive_encode, umi_data_data_operation_archive_decode,
    UmiDataOperation, UmiDataOperationArchiveSchema, UmiDataOperationArchiveBound, UmiDataOperationArchiveWrite, UmiDataOperationArchiveRead, UmiDataOperationArchiveValidate)
